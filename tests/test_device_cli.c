#define _POSIX_C_SOURCE 200809L
#include "device_cli.h"
#include "keymap.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
static unsigned char cfg[128],rgb[512],maps[4][512],pool[4096],perkey[378];
static unsigned opens,cfg_writes,perkey_writes,all_writes,stream_sends;
static int fail_perkey,fail_stream;
static void fixture(void) {
 memset(cfg,0,sizeof(cfg));cfg[10]=6;cfg[126]=0x5a;cfg[127]=0xa5;
 memset(rgb,0,sizeof(rgb));rgb[506]=0x5a;rgb[507]=0xa5;
 memset(maps,0,sizeof(maps));for(unsigned i=0;i<4;i++){maps[i][510]=0x5a;maps[i][511]=0xa5;}
 memset(pool,0,sizeof(pool));memset(perkey,0x55,sizeof(perkey));
 opens=cfg_writes=perkey_writes=all_writes=stream_sends=0;fail_perkey=fail_stream=0;
}
int aula_hid_open(const char *node,int writable,aula_hid_device *d,char *e,size_t n) {
 (void)e;(void)n;assert(!strcmp(node,"/dev/hidraw-test")&&writable==1);memset(d,0,sizeof(*d));opens++;return 0;
}
void aula_hid_close(aula_hid_device *d){(void)d;}
int aula_g5_read_data(aula_hid_device *d,unsigned cmd,unsigned sub,unsigned char *out,size_t len,char *e,size_t n) {
 (void)d;(void)e;(void)n;
 if(cmd==0x83){assert(sub<4&&len==512);memcpy(out,maps[sub],len);}else{assert(cmd==0x85&&sub==0&&len==4096);memcpy(out,pool,len);}return 0;
}
int aula_g5_write_data(aula_hid_device *d,unsigned cmd,unsigned sub,const unsigned char *in,size_t len,char *e,size_t n) {
 (void)d;(void)e;(void)n;all_writes++;
 if(cmd==3){assert(sub<4&&len==512);memcpy(maps[sub],in,len);}else{assert(cmd==5&&sub==0&&len==4096);memcpy(pool,in,len);}return 0;
}
int aula_protocol_read_cfg(aula_hid_device *d,unsigned char out[128],char *e,size_t n){(void)d;(void)e;(void)n;memcpy(out,cfg,128);return 0;}
int aula_protocol_write_cfg(aula_hid_device *d,const unsigned char in[128],char *e,size_t n){(void)d;(void)e;(void)n;cfg_writes++;all_writes++;memcpy(cfg,in,128);return 0;}
int aula_lighting_read_rgb(aula_hid_device *d,unsigned char out[512],char *e,size_t n){(void)d;(void)e;(void)n;memcpy(out,rgb,512);return 0;}
int aula_lighting_write_rgb(aula_hid_device *d,const unsigned char in[512],char *e,size_t n){(void)d;(void)e;(void)n;all_writes++;memcpy(rgb,in,512);return 0;}
int aula_lighting_read_perkey(aula_hid_device *d,unsigned char out[378],char *e,size_t n){(void)d;(void)e;(void)n;memcpy(out,perkey,378);return 0;}
int aula_lighting_write_perkey(aula_hid_device *d,const unsigned char in[378],char *e,size_t n){
 (void)d;perkey_writes++;all_writes++;if(fail_perkey){snprintf(e,n,"mock per-key restoration failure");return -1;}memcpy(perkey,in,378);return 0;
}
int aula_lighting_select_custom(unsigned char out[128],char *e,size_t n){(void)e;(void)n;out[9]=1;out[10]=21;return 0;}
int aula_lighting_send_stream(aula_hid_device *d,const unsigned char frame[378],char *e,size_t n){(void)d;(void)frame;stream_sends++;if(fail_stream){snprintf(e,n,"mock stream failure");return -1;}return 0;}
static char *tohex(const unsigned char *p,size_t len){char *s=malloc(len*2+1);assert(s);for(size_t i=0;i<len;i++)sprintf(s+i*2,"%02x",p[i]);return s;}
static int restore(int invalid_ref){
 unsigned char source_maps[4][512];memcpy(source_maps,maps,sizeof(maps));if(invalid_ref){source_maps[2][80]=3;source_maps[2][81]=1;source_maps[2][82]=1;source_maps[2][83]=0;}
 char *values[]={tohex(cfg,128),tohex(rgb,512),tohex(pool,4096),tohex(perkey,378),tohex(source_maps[0],512),tohex(source_maps[1],512),tohex(source_maps[2],512),tohex(source_maps[3],512)};
 char *args[]={"aula-control","--restore","/dev/hidraw-test","--config",values[0],"--rgb",values[1],"--macros",values[2],"--perkey",values[3],"--map0",values[4],"--map1",values[5],"--map2",values[6],"--map3",values[7]};
 int r=aula_device_cli(sizeof(args)/sizeof(args[0]),args);for(unsigned i=0;i<8;i++)free(values[i]);return r;
}
int main(void){
 assert(freopen("/dev/null","w",stdout));fixture();
 char *snapshot_args[]={"aula-control","--snapshot","/dev/hidraw-test"};assert(!aula_device_cli(3,snapshot_args));assert(opens==1&&all_writes==0);
 assert(!restore(0));assert(cfg_writes==1&&perkey_writes==1&&all_writes==8);
 fixture();assert(restore(1)==2);assert(opens==0&&all_writes==0);
 fixture();int p[2];assert(!pipe(p));unsigned char frame[378]={0};assert(write(p[1],frame,378)==378);close(p[1]);int oldstdin=dup(STDIN_FILENO);assert(oldstdin>=0&&dup2(p[0],STDIN_FILENO)>=0);close(p[0]);
 fail_stream=fail_perkey=1;char *stream_args[]={"aula-control","--stream-stdin","/dev/hidraw-test"};int result=aula_device_cli(3,stream_args);assert(dup2(oldstdin,STDIN_FILENO)>=0);close(oldstdin);
 assert(result==1&&stream_sends==1&&perkey_writes==1&&cfg_writes==2&&cfg[9]==0&&cfg[10]==6);
 fputs("device CLI tests passed\n",stderr);return 0;
}
