#define _POSIX_C_SOURCE 200809L
#include "device_cli.h"
#include "keymap.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static void hex(const unsigned char *p,size_t n) {for(size_t i=0;i<n;i++)printf("%02x",p[i]);}
static int unhex(const char *s,unsigned char *p,size_t n) {
 if(!s||strlen(s)!=n*2)return -1;
 for(size_t i=0;i<n;i++) {
  unsigned value=0;
  for(unsigned j=0;j<2;j++) {
   unsigned char c=(unsigned char)s[i*2+j];unsigned d;
   if(c>='0'&&c<='9')d=c-'0';else if(c>='a'&&c<='f')d=c-'a'+10;
   else if(c>='A'&&c<='F')d=c-'A'+10;else return -1;
   value=value*16+d;
  }
  p[i]=(unsigned char)value;
 }
 return 0;
}
static void pause_ms(long n) {struct timespec t={n/1000,(n%1000)*1000000};while(nanosleep(&t,&t)<0&&errno==EINTR){}}
static volatile sig_atomic_t stopped;
static void stop(int sig) {(void)sig;stopped=1;}
typedef struct {unsigned char cfg[128],rgb[512],map[4][512],macro[4096],perkey[378];} snapshot;
static int get_snapshot(aula_hid_device *d,snapshot *s,char *e,size_t n) {
 if(aula_protocol_read_cfg(d,s->cfg,e,n)||aula_lighting_read_rgb(d,s->rgb,e,n)||
    aula_macro_read(d,s->macro,e,n)||aula_lighting_read_perkey(d,s->perkey,e,n))return -1;
 for(unsigned l=0;l<4;l++)if(aula_keymap_read(d,l,s->map[l],e,n))return -1;
 return 0;
}
static int put_snapshot(aula_hid_device *d,const snapshot *s,char *e,size_t n) {
 if(aula_macro_write(d,s->macro,e,n)||aula_lighting_write_rgb(d,s->rgb,e,n))return -1;
 for(unsigned l=0;l<4;l++)if(aula_keymap_write(d,l,s->map[l],e,n))return -1;
 if(aula_lighting_write_perkey(d,s->perkey,e,n))return -1;
 return aula_protocol_write_cfg(d,s->cfg,e,n);
}
int aula_device_cli(int argc,char **argv) {
 if(argc<2)return -1;
 int paint=!strcmp(argv[1],"--paint"),read_cmd=!strcmp(argv[1],"--perkey");
 int stream=!strcmp(argv[1],"--stream-stdin"),snap=!strcmp(argv[1],"--snapshot"),restore=!strcmp(argv[1],"--restore");
 if(!paint&&!read_cmd&&!stream&&!snap&&!restore)return -1;
 char error[512]={0};if(argc<3){fprintf(stderr,"A wired device node is required.\n");return 2;}
 unsigned char colors[378],oldcolors[378],cfg[128],oldcfg[128];snapshot state,before;
 if((read_cmd||stream||snap)&&argc!=3)goto invalid;
 if(paint) {
  const char *data=NULL;
  for(int i=3;i<argc;i+=2) {
   if(i+1==argc)goto invalid;
   if(!strcmp(argv[i],"--model")) {if(strcmp(argv[i+1],"F75")&&strcmp(argv[i+1],"F87")&&strcmp(argv[i+1],"F99"))goto invalid;}
   else if(!strcmp(argv[i],"--colors")&&!data)data=argv[i+1];else goto invalid;
  }
  unsigned char interleaved[378];if(unhex(data,interleaved,378))goto invalid;
  for(unsigned i=0;i<126;i++)for(unsigned c=0;c<3;c++)colors[c*126+i]=interleaved[i*3+c];
 }
 if(restore) {
  unsigned seen=0;
  for(int i=3;i<argc;i+=2) {
   if(i+1==argc)goto invalid;
   unsigned bit;unsigned char *p;size_t n;
   if(!strcmp(argv[i],"--config")){bit=1;p=state.cfg;n=128;}
   else if(!strcmp(argv[i],"--rgb")){bit=2;p=state.rgb;n=512;}
   else if(!strcmp(argv[i],"--macros")){bit=4;p=state.macro;n=4096;}
   else if(!strcmp(argv[i],"--perkey")){bit=8;p=state.perkey;n=378;}
   else if(!strncmp(argv[i],"--map",5)&&strlen(argv[i])==6&&argv[i][5]>='0'&&argv[i][5]<='3') {
    unsigned l=(unsigned)(argv[i][5]-'0');bit=16u<<l;p=state.map[l];n=512;
   } else goto invalid;
   if((seen&bit)||unhex(argv[i+1],p,n))goto invalid;
   seen|=bit;
  }
  if(seen!=255||state.cfg[126]!=0x5a||state.cfg[127]!=0xa5||
     !((state.rgb[506]==0x5a&&state.rgb[507]==0xa5)||(state.rgb[510]==0x5a&&state.rgb[511]==0xa5)))goto invalid;
  unsigned count;size_t used;
  if(aula_macro_count(state.macro,&count,&used,error,sizeof(error)))goto invalid;
  for(unsigned l=0;l<4;l++) {
   if(state.map[l][510]!=0x5a||state.map[l][511]!=0xa5)goto invalid;
   for(unsigned i=0;i<127;i++)if(state.map[l][i*4]==3&&state.map[l][i*4+3]>=count)goto invalid;
  }
 }
 aula_hid_device dev;if(aula_hid_open(argv[2],1,&dev,error,sizeof(error)))goto fail;
 int r=0;
 if(read_cmd) {
  r=aula_lighting_read_perkey(&dev,colors,error,sizeof(error));
  if(!r)r=aula_protocol_read_cfg(&dev,cfg,error,sizeof(error));
  if(!r){printf("{\"custom\":%s,\"colors\":[",cfg[9]&&cfg[10]==21?"true":"false");
   for(unsigned i=0;i<126;i++)printf("%s\"#%02x%02x%02x\"",i?",":"",colors[i],colors[i+126],colors[i+252]);
   puts("]}");}
 } else if(paint) {
  r=aula_protocol_read_cfg(&dev,oldcfg,error,sizeof(error));
  if(!r)r=aula_lighting_read_perkey(&dev,oldcolors,error,sizeof(error));
  if(!r) {
   memcpy(cfg,oldcfg,128);r=aula_lighting_select_custom(cfg,error,sizeof(error));
   if(!r)r=aula_protocol_write_cfg(&dev,cfg,error,sizeof(error));
   if(!r){pause_ms(400);r=aula_lighting_write_perkey(&dev,colors,error,sizeof(error));}
   if(r){char rollback[256];
    if(aula_lighting_write_perkey(&dev,oldcolors,rollback,sizeof(rollback)))fprintf(stderr,"Custom-color rollback failed: %s\n",rollback);
    if(aula_protocol_write_cfg(&dev,oldcfg,rollback,sizeof(rollback)))fprintf(stderr,"Configuration rollback failed: %s\n",rollback);
   }
  }
  if(!r)puts("{\"verified\":true,\"mode\":21}");
 } else if(snap) {
  r=get_snapshot(&dev,&state,error,sizeof(error));
  if(!r){printf("{\"format\":\"aula-wired-snapshot-v1\",\"config_hex\":\"");hex(state.cfg,128);
   printf("\",\"rgb_hex\":\"");hex(state.rgb,512);printf("\",\"keymaps_hex\":[");
   for(unsigned l=0;l<4;l++){printf("%s\"",l?",":"");hex(state.map[l],512);putchar('"');}
   printf("],\"macros_hex\":\"");hex(state.macro,4096);printf("\",\"perkey_hex\":\"");hex(state.perkey,378);puts("\"}");}
 } else if(restore) {
  r=get_snapshot(&dev,&before,error,sizeof(error));
  if(!r){r=put_snapshot(&dev,&state,error,sizeof(error));
   if(r){char ignored[256];if(put_snapshot(&dev,&before,ignored,sizeof(ignored)))fprintf(stderr,"Rollback failed: %s\n",ignored);}}
  if(!r)puts("{\"verified\":true,\"restored\":true}");
 } else {
  r=aula_protocol_read_cfg(&dev,oldcfg,error,sizeof(error));
  if(!r)r=aula_lighting_read_perkey(&dev,oldcolors,error,sizeof(error));
  if(!r) {
   struct sigaction sa;memset(&sa,0,sizeof(sa));sa.sa_handler=stop;sigemptyset(&sa.sa_mask);
   sigaction(SIGTERM,&sa,NULL);sigaction(SIGINT,&sa,NULL);stopped=0;
   memcpy(cfg,oldcfg,128);aula_lighting_select_custom(cfg,error,sizeof(error));
   r=aula_protocol_write_cfg(&dev,cfg,error,sizeof(error));pause_ms(400);
   unsigned char frame[378];size_t have=0;
   while(!r&&!stopped){ssize_t n=read(STDIN_FILENO,frame+have,sizeof(frame)-have);
    if(n<0){if(errno==EINTR)continue;snprintf(error,sizeof(error),"Cannot read animation frame");r=-1;break;}
    if(!n)break;
    have+=(size_t)n;
    if(have==sizeof(frame)){r=aula_lighting_send_stream(&dev,frame,error,sizeof(error));have=0;pause_ms(30);}
   }
   char restore_error[256]={0};
   int rr=aula_lighting_write_perkey(&dev,oldcolors,restore_error,sizeof(restore_error));
   if(rr)fprintf(stderr,"Color restore after animation failed: %s\n",restore_error);
   int cr=aula_protocol_write_cfg(&dev,oldcfg,restore_error,sizeof(restore_error));
   if(cr)fprintf(stderr,"Configuration restore after animation failed: %s\n",restore_error);
   if(rr||cr){snprintf(error,sizeof(error),"Could not fully restore keyboard state after animation; read the keyboard before another operation.");r=-1;}
   if(!r)puts("{\"stopped\":true,\"restored\":true}");
  }
 }
 aula_hid_close(&dev);if(r)goto fail;return 0;
invalid:fprintf(stderr,"Invalid per-key, stream or snapshot arguments.\n");return 2;
fail:fprintf(stderr,"%s\n",error);return 1;
}
