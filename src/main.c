#define _GNU_SOURCE
#include "hid_linux.h"
#include "protocol.h"
#include "lighting_ext.h"
#include "keymap_cli.h"
#include "device_cli.h"
#include "gui_embedded.h"
#include "permissions_embedded.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static const char *modes[] = {"Off", "Fixed", "Breathing", "Rainbow", "Flash away",
 "Raindrops", "Rainbow wheel", "Ripples", "Stars", "Shadow", "Retro snake",
 "Neon stream", "Reaction", "Sine wave", "Retinue scanning", "Windmill",
 "Waterfall", "Blossoming", "Storm"};
static void json_string(const char *s) {
 putchar('"');
 for (const unsigned char *p=(const unsigned char *)s; *p; ++p) {
  if (*p=='"' || *p=='\\') {putchar('\\'); putchar(*p);}
  else if (*p<32 || *p>=127) printf("\\u%04x", *p);
  else putchar(*p);
 }
 putchar('"');
}
static void found(const aula_hid_info *d, void *state) {
 int *count=state;
 if ((*count)++) putchar(',');
 printf("{\"node\":"); json_string(d->node);
 printf(",\"name\":"); json_string(d->name);
 printf(",\"vendor\":\"%04x\",\"product\":\"%04x\",",d->vendor,d->product);
 printf("\"node_exists\":%s,\"readable\":%s,\"writable\":%s,",
  d->node_exists?"true":"false",d->readable?"true":"false",d->writable?"true":"false");
 int wired=d->vendor==0x258a;
 int supported=wired && d->descriptor_ok && d->reports.feature[6].present &&
                d->reports.feature[6].ioctl_bytes==520;
 printf("\"transport\":\"%s\",\"config_supported\":%s,\"features\":[",
        wired?"USB cable":"2.4 GHz receiver",supported?"true":"false");
 int n=0;
 for (unsigned i=0;i<256;++i) if(d->reports.feature[i].present) {
  if(n++) putchar(',');
  printf("{\"id\":%u,\"bytes\":%zu,\"usage_page\":%u}",i,
    d->reports.feature[i].ioctl_bytes,d->reports.feature[i].usage_page);
 }
 printf("]}");
}
static void show_cfg(const unsigned char *cfg,const unsigned char *rgb,const char *model,int verified) {
 unsigned mode=cfg[10];
 int brightness=-1,speed=-1;
 if(mode>=1 && mode<=18) {
  unsigned off=0x3a+2*(mode-1);
  brightness=cfg[off]; speed=cfg[off+1]>>4;
 }
 printf("{\"mode\":%u,\"brightness\":%d,\"speed\":%d,\"verified\":%s,\"config_hex\":\"",
       mode,brightness,speed,verified?"true":"false");
 for(unsigned i=0;i<128;++i) printf("%02x",cfg[i]);
 printf("\",\"charging_light\":%s,\"sleep_seconds\":%u,\"debounce_ms\":%u,\"rainbow\":%s",
        cfg[0x12]?"true":"false",(unsigned)cfg[0x18]*30,(unsigned)cfg[3]+1,
        mode>=1&&mode<=18&&(cfg[0x3b+2*(mode-1)]&15)==7?"true":"false");
 if(rgb&&mode>=1&&mode<=18) {
  unsigned selection=cfg[0x3b+2*(mode-1)]&15;if(selection>6)selection=0;
  unsigned at=mode*21+selection*3;
  printf(",\"color\":\"#%02x%02x%02x\",\"palette\":[",rgb[at],rgb[at+1],rgb[at+2]);
  for(unsigned i=0;i<7;i++){at=mode*21+i*3;printf("%s\"#%02x%02x%02x\"",i?",":"",rgb[at],rgb[at+1],rgb[at+2]);}
  putchar(']');
 }
 if(model){printf(",\"model\":");json_string(model);}
 puts("}");
}
static int parse_color(const char *s,unsigned char rgb[3]) {
 if(*s=='#')s++;
 if(strlen(s)!=6)return -1;
 for(unsigned i=0;i<3;i++){unsigned value=0;
  for(unsigned j=0;j<2;j++){unsigned char c=(unsigned char)*s++;unsigned d;
   if(c>='0'&&c<='9')d=c-'0';else if(c>='a'&&c<='f')d=c-'a'+10;else if(c>='A'&&c<='F')d=c-'A'+10;else return -1;
   value=value*16+d;}
  rgb[i]=(unsigned char)value;}
 return 0;
}
static int number(const char *s,int max,int *out) {
 char *end; errno=0; long v=strtol(s,&end,10);
 if(errno || !*s || *end || v<0 || v>max) return -1;
 *out=(int)v; return 0;
}
static void help(void) {
 puts("AULA F Series Control 0.2.1 — native Linux keyboard configuration\n"
 "  aula-control                     Open desktop interface\n"
 "  aula-control --list               Discover supported HID interfaces (JSON)\n"
 "  aula-control --modes              List lighting effect IDs (JSON)\n"
 "  aula-control --setup-permissions  Install wired access rule (admin authentication)\n"
 "  aula-control --read /dev/hidrawN   Read 128-byte lighting configuration\n"
 "  aula-control --apply /dev/hidrawN --model F75|F87|F99 [settings]\n"
 "  aula-control --preview --model F75|F87|F99 [settings]\n"
 "Settings: --mode 0..18 --brightness 0..9 (F75), 0..4 (F87/F99) --speed 0..4\n"
 "          --color RRGGBB --rainbow 0|1 --palette HEX,HEX,... (seven colors)\n"
 "          --charging-light 0|1 --sleep SECONDS --debounce 1..5\n"
 "  --keymap NODE --layer 0..3 | --remap NODE --layer N --key INDEX --action TYPE --code VALUE\n"
 "  --macro-list NODE | --macro-add NODE --name NAME --events FILE --layer N --key INDEX\n"
 "  --perkey NODE | --paint NODE --model F75 --colors INTERLEAVED_RGB_HEX\n"
 "  --snapshot NODE | --restore NODE --config HEX --rgb HEX --map0 HEX ... --map3 HEX --macros HEX --perkey HEX\n"
 "  --stream-stdin NODE  Read 378-byte interleaved RGB frames from stdin; restores state on exit.\n"
 "Read/apply currently require USB cable 258a:010c, feature report 6 (520 bytes).\n"
 "Wireless is discovered; its configuration transport is not implemented.\n"
 "Wired read/write was verified on the connected keyboard. Apply reads, patches,\n"
 "writes and verifies; other models and firmware versions still need validation.\n"
 "Firmware updating and Bluetooth configuration are not implemented.");
}
int main(int argc,char **argv) {
 char error[512]={0};
 int handled=aula_keymap_cli(argc,argv);if(handled>=0)return handled;
 handled=aula_device_cli(argc,argv);if(handled>=0)return handled;
 if(argc==2 && !strcmp(argv[1],"--setup-permissions")) {
  execl("/usr/bin/pkexec","pkexec","/usr/bin/python3","-I","-c",aula_permissions_source,(char *)NULL);
  perror("Unable to start administrator authentication with pkexec");
  return 1;
 }
 if(argc==1 || (argc==2 && !strcmp(argv[1],"--gui"))) {
  char exe[PATH_MAX]; ssize_t n=readlink("/proc/self/exe",exe,sizeof(exe)-1);
  if(n<0) {perror("readlink"); return 1;} exe[n]=0;
  /* A single exec argument is limited to 128 KiB, which the interface source
     exceeds, so hand it to Python through an in-memory file instead of -c. */
  int fd=memfd_create("aula-studio.py",0);
  size_t left=strlen(aula_gui_source); const char *at=aula_gui_source;
  while(fd>=0 && left) {ssize_t w=write(fd,at,left); if(w<=0) {close(fd); fd=-1;} else {at+=w; left-=(size_t)w;}}
  if(fd>=0) {
   char script[64]; snprintf(script,sizeof(script),"/proc/self/fd/%d",fd);
   execl("/usr/bin/python3","python3",script,exe,(char *)NULL);
  }
  execl("/usr/bin/python3","python3","-c",aula_gui_source,exe,(char *)NULL);
  perror("Unable to start desktop UI with /usr/bin/python3 (PyGObject/GTK3 required)");
  return 1;
 }
 if(argc==2 && (!strcmp(argv[1],"--help") || !strcmp(argv[1],"-h"))) {help();return 0;}
 if(argc==2 && !strcmp(argv[1],"--version")) {puts("aula-control 0.2.1 (native Linux keyboard configuration)");return 0;}
 if(argc==2 && !strcmp(argv[1],"--list")) {
  int count=0; putchar('[');
  int r=aula_hid_discover(found,&count,error,sizeof(error)); puts("]");
  if(r<0) {fprintf(stderr,"%s\n",error);return 1;}return 0;
 }
 if(argc==2 && !strcmp(argv[1],"--modes")) {
  putchar('['); for(unsigned i=0;i<19;++i) {
   if(i) putchar(',');
   printf("{\"id\":%u,\"name\":",i);json_string(modes[i]);putchar('}');
  } puts("]");return 0;
 }
 int read_cmd=!strcmp(argv[1],"--read"), apply=!strcmp(argv[1],"--apply");
 int preview=!strcmp(argv[1],"--preview");
 if(!read_cmd && !apply && !preview) {help();return 2;}
 int mode=-1,bright=-1,speed=-1,indicator=-1,sleep=-1,debounce=-1,rainbow=-1;
 unsigned char color[3],palette[21];int has_color=0,has_palette=0;
 const char *node=NULL,*model=NULL;
 int index=2;
 if(read_cmd || apply) {if(argc<3) goto invalid;node=argv[index++];}
 while(index<argc) {
  const char *arg=argv[index++]; if(index==argc) goto invalid;
  const char *val=argv[index++];
  if(!strcmp(arg,"--model") && !model) model=val;
  else if(!strcmp(arg,"--mode") && mode==-1) {if(number(val,18,&mode)) goto invalid;}
  else if(!strcmp(arg,"--brightness") && bright==-1) {if(number(val,9,&bright)) goto invalid;}
  else if(!strcmp(arg,"--speed") && speed==-1) {if(number(val,4,&speed)) goto invalid;}
  else if(!strcmp(arg,"--charging-light")&&indicator==-1){if(number(val,1,&indicator))goto invalid;}
  else if(!strcmp(arg,"--sleep")&&sleep==-1){if(number(val,1200,&sleep))goto invalid;}
  else if(!strcmp(arg,"--debounce")&&debounce==-1){if(number(val,5,&debounce)||!debounce)goto invalid;}
  else if(!strcmp(arg,"--rainbow")&&rainbow==-1){if(number(val,1,&rainbow))goto invalid;}
  else if(!strcmp(arg,"--color")&&!has_color){if(parse_color(val,color))goto invalid;has_color=1;}
  else if(!strcmp(arg,"--palette")&&!has_palette){
   if(strlen(val)!=48)goto invalid;
   for(unsigned i=0;i<7;i++){char hexcolor[7];memcpy(hexcolor,val+i*7,6);hexcolor[6]=0;
    if(parse_color(hexcolor,palette+i*3)||(i<6&&val[i*7+6]!=','))goto invalid;}
   has_palette=1;
  }
  else goto invalid;
 }
 if(read_cmd && (argc!=3)) goto invalid;
 if(!read_cmd && (!model || (strcmp(model,"F75") && strcmp(model,"F87") && strcmp(model,"F99")))) goto invalid;
 if(!read_cmd && mode==-1 && bright==-1 && speed==-1 &&indicator==-1&&sleep==-1&&debounce==-1&&rainbow==-1&&!has_color&&!has_palette) goto invalid;
 if(model && strcmp(model,"F75") && bright>4) goto invalid;
 if(preview) {
  printf("{\"model\":");json_string(model);
  printf(",\"mode\":%d,\"brightness\":%d,\"speed\":%d,",mode,bright,speed);
  printf("\"read_packet_prefix\":[6,132,0,0,1,0,128,0],\"config_patches\":[");
  int p=0;
  if(mode>=0) {printf("{\"offset\":9,\"value\":0,\"mask\":255},{\"offset\":10,\"value\":%d,\"mask\":255}",mode);p=1;}
  if(mode>0) {
   if(bright>=0) {printf("%s{\"offset\":%d,\"value\":%d,\"mask\":255}",p?",":"",0x3a+2*(mode-1),bright);p=1;}
   if(speed>=0) printf("%s{\"offset\":%d,\"value\":%d,\"mask\":240}",p?",":"",0x3b+2*(mode-1),speed<<4);
  }
  puts("],\"note\":\"Preview only. Full packet requires a fresh configuration read. Brightness/speed use the selected or current effect; lighting writes also set offsets 56 and 57 to 255.\"}");
  return 0;
 }
 aula_hid_device dev;
 if(aula_hid_open(node,1,&dev,error,sizeof(error))) goto fail;
 unsigned char cfg[128],original_cfg[128],rgb[512],original_rgb[512];int rgb_ok=0;
 const char *identified=NULL;
 int r=aula_protocol_read_cfg(&dev,cfg,error,sizeof(error));
 if(!r){memcpy(original_cfg,cfg,128);
  char rgb_error[256];rgb_ok=!aula_lighting_read_rgb(&dev,rgb,rgb_error,sizeof(rgb_error));
  if(rgb_ok)memcpy(original_rgb,rgb,512);
  else if(has_color||has_palette){snprintf(error,sizeof(error),"Cannot read RGB palette: %s",rgb_error);r=-1;}
  unsigned char psd[6];char identify_error[256];
  if(!aula_g5_read_data(&dev,0x82,1,psd,sizeof(psd),identify_error,sizeof(identify_error))&&
     psd[0]==3&&psd[1]==0&&psd[2]==0&&psd[3]==0&&psd[4]==3) {
   if(psd[5]==0xc2)identified="F75";else if(psd[5]==0xc1)identified="F87";else if(psd[5]==0xc3)identified="F99";
  }
  if(apply&&identified&&strcmp(model,identified)){snprintf(error,sizeof(error),"Keyboard identifies as %s; choose that model before applying.",identified);r=-1;}
 }
 if(!r && apply) {
  enum aula_model m=!strcmp(model,"F75")?AULA_MODEL_F75:!strcmp(model,"F87")?AULA_MODEL_F87:AULA_MODEL_F99;
  r=aula_protocol_patch_cfg(cfg,m,mode,bright,speed,error,sizeof(error));
  if(!r)r=aula_lighting_patch_settings(cfg,m,debounce,sleep,error,sizeof(error));
  if(!r)r=aula_lighting_patch_indicator(cfg,indicator,error,sizeof(error));
  int selected=cfg[10];
  if(!r&&selected!=0&&(has_color||has_palette)) {
   if(has_palette)r=aula_lighting_patch_colors(rgb,selected,palette,error,sizeof(error));
   if(!r&&has_color)r=aula_lighting_patch_palette(rgb,selected,0,color[0],color[1],color[2],error,sizeof(error));
   if(!r&&has_color&&rainbow!=1)r=aula_lighting_patch_color_selection(cfg,selected,0,error,sizeof(error));
  }
  if(!r&&selected!=0&&rainbow!=-1)r=aula_lighting_patch_color_selection(cfg,selected,rainbow?7:0,error,sizeof(error));
  int rgb_written=0,cfg_written=0;
  if(!r&&rgb_ok&&memcmp(rgb,original_rgb,512)) {rgb_written=1;r=aula_lighting_write_rgb(&dev,rgb,error,sizeof(error));}
  if(!r&&memcmp(cfg,original_cfg,128)){cfg_written=1;r=aula_protocol_write_cfg(&dev,cfg,error,sizeof(error));}
  if(r&&rgb_written){char rollback[256];if(aula_lighting_write_rgb(&dev,original_rgb,rollback,sizeof(rollback)))fprintf(stderr,"Color rollback failed: %s\n",rollback);}
  if(r&&cfg_written){char rollback[256];if(aula_protocol_write_cfg(&dev,original_cfg,rollback,sizeof(rollback)))fprintf(stderr,"Configuration rollback failed: %s\n",rollback);}
 }
 aula_hid_close(&dev);
 if(r) goto fail;
 show_cfg(cfg,rgb_ok?rgb:NULL,identified,apply); return 0;
invalid:
 fprintf(stderr,"Invalid or missing arguments. Run --help for usage.\n");return 2;
fail:
 fprintf(stderr,"%s\n",error);return 1;
}
