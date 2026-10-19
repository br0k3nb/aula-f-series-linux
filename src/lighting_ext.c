#define _POSIX_C_SOURCE 200809L
#include "lighting_ext.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int fail(char *e,size_t n,const char *fmt,...) {
    va_list ap; va_start(ap,fmt);
    if(e && n) vsnprintf(e,n,fmt,ap);
    va_end(ap); return -1;
}
static int pause20(char *e,size_t en) {
    struct timespec delay={0,20000000L};
    while(nanosleep(&delay,&delay)<0) {
        if(errno!=EINTR) return fail(e,en,"Cannot wait for device: %s",strerror(errno));
    }
    return 0;
}
static int check_device(const aula_hid_device *d,char *e,size_t en) {
    if(!d || d->fd<0 || d->vendor!=0x258a || d->product!=0x010c)
        return fail(e,en,"Protocol requires wired AULA 258a:010c; wireless is unsupported");
    if(!d->writable) return fail(e,en,"Feature requests require a writable device handle");
    if(!d->reports.feature[AULA_FEATURE_ID].present ||
       d->reports.feature[AULA_FEATURE_ID].ioctl_bytes!=AULA_PACKET_SIZE)
        return fail(e,en,"Device must declare feature report 6 with 520 ioctl bytes");
    return 0;
}
static int check_cfg(const unsigned char *cfg,char *e,size_t en) {
    if(!cfg || cfg[126]!=0x5a || cfg[127]!=0xa5)
        return fail(e,en,"Configuration is missing required 5a a5 tail markers");
    return 0;
}
static int check_rgb(const unsigned char *rgb,char *e,size_t en) {
    /* The connected F75 places its palette marker at 506, while keymaps
     * use 510. Preserve whichever layout was read; never move the marker. */
    if(!rgb || !((rgb[506]==0x5a && rgb[507]==0xa5) || (rgb[510]==0x5a && rgb[511]==0xa5)))
        return fail(e,en,"RGB table is missing required 5a a5 tail markers");
    return 0;
}
int aula_g5_build_packet(unsigned cmd,unsigned sub,unsigned total,unsigned index,
                         const unsigned char *payload,size_t length,
                         unsigned char packet[AULA_PACKET_SIZE],char *e,size_t en) {
    if(!packet || cmd>255 || !cmd || sub>255 || !total || total>255 ||
       index>=total || !length || length>AULA_G5_CHUNK_SIZE)
        return fail(e,en,"Invalid G5 chunk header or payload length");
    memset(packet,0,AULA_PACKET_SIZE);
    packet[0]=AULA_FEATURE_ID; packet[1]=(unsigned char)cmd;
    packet[2]=(unsigned char)sub; packet[4]=(unsigned char)total;
    packet[5]=(unsigned char)index;
    packet[6]=(unsigned char)length; packet[7]=(unsigned char)(length>>8);
    if(payload) memcpy(packet+8,payload,length);
    return 0;
}
static int check_transfer(aula_hid_device *d,unsigned cmd,unsigned sub,
                           const unsigned char *data,size_t length,int reading,
                           char *e,size_t en) {
    if(check_device(d,e,en)<0) return -1;
    if(!data || !length || length>AULA_G5_MAX_DATA || sub>255 || !cmd || cmd>255 ||
       (reading ? !(cmd&0x80u) : (cmd&0x80u)))
        return fail(e,en,"Invalid G5 transfer command, data or length");
    return 0;
}
int aula_g5_read_data(aula_hid_device *d,unsigned cmd,unsigned sub,
                      unsigned char *output,size_t length,char *e,size_t en) {
    unsigned char request[AULA_PACKET_SIZE],reply[AULA_PACKET_SIZE],*temporary;
    unsigned total,index=0; size_t offset=0;
    if(check_transfer(d,cmd,sub,output,length,1,e,en)<0) return -1;
    temporary=malloc(length);
    if(!temporary) return fail(e,en,"Cannot allocate G5 read buffer");
    total=(unsigned)((length+AULA_G5_CHUNK_SIZE-1)/AULA_G5_CHUNK_SIZE);
    while(offset<length) {
        size_t chunk=length-offset; int n;
        if(chunk>AULA_G5_CHUNK_SIZE) chunk=AULA_G5_CHUNK_SIZE;
        if(aula_g5_build_packet(cmd,sub,total,index,NULL,chunk,request,e,en)<0 ||
           pause20(e,en)<0) goto error;
        n=aula_hid_set_feature(d,AULA_FEATURE_ID,request,sizeof(request),e,en);
        if(n<0) goto error;
        if(n!=(int)sizeof(request)) {
            fail(e,en,"Short G5 read request: %d bytes",n); goto error;
        }
        if(pause20(e,en)<0) goto error;
        memset(reply,0,sizeof(reply));
        n=aula_hid_get_feature(d,AULA_FEATURE_ID,reply,sizeof(reply),e,en);
        if(n<0) goto error;
        if(n<(int)(8+chunk) || n>(int)sizeof(reply)) {
            fail(e,en,"Incomplete G5 response for chunk %u: %d bytes (need at least %zu)",index,n,8+chunk);
            goto error;
        }
        if(memcmp(reply,request,8)) {
            fail(e,en,"Unexpected G5 response header for chunk %u",index); goto error;
        }
        memcpy(temporary+offset,reply+8,chunk);
        offset+=chunk; index++;
    }
    memcpy(output,temporary,length); free(temporary); return 0;
error:
    free(temporary); return -1;
}
int aula_g5_write_data(aula_hid_device *d,unsigned cmd,unsigned sub,
                       const unsigned char *payload,size_t length,char *e,size_t en) {
    unsigned char packet[AULA_PACKET_SIZE]; unsigned total,index=0; size_t offset=0;
    if(check_transfer(d,cmd,sub,payload,length,0,e,en)<0) return -1;
    total=(unsigned)((length+AULA_G5_CHUNK_SIZE-1)/AULA_G5_CHUNK_SIZE);
    while(offset<length) {
        size_t chunk=length-offset; int n;
        if(chunk>AULA_G5_CHUNK_SIZE) chunk=AULA_G5_CHUNK_SIZE;
        if(aula_g5_build_packet(cmd,sub,total,index,payload+offset,chunk,packet,e,en)<0 ||
           pause20(e,en)<0) return -1;
        n=aula_hid_set_feature(d,AULA_FEATURE_ID,packet,sizeof(packet),e,en);
        if(n<0) return -1;
        if(n!=(int)sizeof(packet)) return fail(e,en,"Short G5 write for chunk %u: %d bytes",index,n);
        offset+=chunk; index++;
    }
    return 0;
}
int aula_lighting_read_rgb(aula_hid_device *d,unsigned char rgb[AULA_RGB_SIZE],char *e,size_t en) {
    unsigned char temporary[AULA_RGB_SIZE];
    if(!rgb) return fail(e,en,"Missing RGB output buffer");
    if(aula_g5_read_data(d,0x8a,0,temporary,sizeof(temporary),e,en)<0 ||
       check_rgb(temporary,e,en)<0) return -1;
    memcpy(rgb,temporary,sizeof(temporary)); return 0;
}
int aula_lighting_write_rgb(aula_hid_device *d,const unsigned char rgb[AULA_RGB_SIZE],char *e,size_t en) {
    unsigned char verify[AULA_RGB_SIZE];
    if(check_rgb(rgb,e,en)<0 || aula_g5_write_data(d,0x0a,0,rgb,AULA_RGB_SIZE,e,en)<0 ||
       aula_lighting_read_rgb(d,verify,e,en)<0) return -1;
    if(memcmp(rgb,verify,AULA_RGB_SIZE))
        return fail(e,en,"RGB read-back differs; device may already have applied part of the change");
    return 0;
}
int aula_lighting_read_perkey(aula_hid_device *d,unsigned char colors[AULA_PERKEY_SIZE],char *e,size_t en) {
    return aula_g5_read_data(d,0x86,0,colors,AULA_PERKEY_SIZE,e,en);
}
int aula_lighting_write_perkey(aula_hid_device *d,const unsigned char colors[AULA_PERKEY_SIZE],char *e,size_t en) {
    unsigned char verify[AULA_PERKEY_SIZE];
    if(aula_g5_write_data(d,6,0,colors,AULA_PERKEY_SIZE,e,en)<0 ||
       aula_lighting_read_perkey(d,verify,e,en)<0) return -1;
    if(memcmp(colors,verify,AULA_PERKEY_SIZE))
        return fail(e,en,"Per-key color read-back differs; device may already have applied part of the change");
    return 0;
}
int aula_lighting_patch_perkey(unsigned char colors[AULA_PERKEY_SIZE],unsigned matrix_index,
                               unsigned red,unsigned green,unsigned blue,char *e,size_t en) {
    if(!colors || matrix_index>=AULA_PERKEY_COUNT || red>255 || green>255 || blue>255)
        return fail(e,en,"Invalid per-key matrix index or RGB component");
    colors[matrix_index]=(unsigned char)red;
    colors[matrix_index+AULA_PERKEY_COUNT]=(unsigned char)green;
    colors[matrix_index+2*AULA_PERKEY_COUNT]=(unsigned char)blue;
    return 0;
}
int aula_lighting_select_custom(unsigned char cfg[AULA_CFG_SIZE],char *e,size_t en) {
    if(check_cfg(cfg,e,en)<0) return -1;
    cfg[9]=1; cfg[10]=21; return 0;
}
int aula_lighting_send_stream(aula_hid_device *d,const unsigned char frame[AULA_PERKEY_SIZE],char *e,size_t en) {
    return aula_g5_write_data(d,8,0,frame,AULA_PERKEY_SIZE,e,en);
}
int aula_lighting_patch_indicator(unsigned char cfg[AULA_CFG_SIZE],int on,char *e,size_t en) {
    if(check_cfg(cfg,e,en)<0) return -1;
    if(on < -1 || on>1) return fail(e,en,"Charging light must be on or off");
    if(on!=-1) cfg[0x12]=(unsigned char)on;
    return 0;
}
static int check_mode(int mode,char *e,size_t en) {
    if(mode<1 || mode>18) return fail(e,en,"Palette requires a hardware effect from 1 through 18");
    return 0;
}
int aula_lighting_patch_palette(unsigned char rgb[AULA_RGB_SIZE],int mode,int color_index,
                                unsigned red,unsigned green,unsigned blue,char *e,size_t en) {
    size_t offset;
    if(check_rgb(rgb,e,en)<0 || check_mode(mode,e,en)<0) return -1;
    if(color_index<0 || color_index>=7 || red>255 || green>255 || blue>255)
        return fail(e,en,"Invalid palette color or RGB component");
    offset=(size_t)mode*21u+(unsigned)color_index*3u;
    rgb[offset]=(unsigned char)red; rgb[offset+1]=(unsigned char)green;
    rgb[offset+2]=(unsigned char)blue; return 0;
}
int aula_lighting_patch_colors(unsigned char rgb[AULA_RGB_SIZE],int mode,
                               const unsigned char colors[21],char *e,size_t en) {
    if(check_rgb(rgb,e,en)<0 || check_mode(mode,e,en)<0) return -1;
    if(!colors) return fail(e,en,"Missing RGB palette");
    memmove(rgb+(size_t)mode*21u,colors,21); return 0;
}
int aula_lighting_patch_color_selection(unsigned char cfg[AULA_CFG_SIZE],int mode,
                                         int color_index,char *e,size_t en) {
    size_t offset;
    if(check_cfg(cfg,e,en)<0 || check_mode(mode,e,en)<0) return -1;
    if(color_index<0 || color_index>7) return fail(e,en,"Palette selection must be 0..7");
    offset=0x3b+2u*(unsigned)(mode-1);
    cfg[offset]=(unsigned char)((cfg[offset]&0xf0)|(unsigned)color_index); return 0;
}
int aula_lighting_patch_settings(unsigned char cfg[AULA_CFG_SIZE],enum aula_model model,
                                  int debounce_ms,int sleep_seconds,char *e,size_t en) {
    static const int sleep_values[]={0,60,90,120,180,240,300,600,900,1200};
    int sleep_ok=sleep_seconds==-1;
    if(check_cfg(cfg,e,en)<0) return -1;
    if(model!=AULA_MODEL_F75 && model!=AULA_MODEL_F87 && model!=AULA_MODEL_F99)
        return fail(e,en,"Unknown keyboard model");
    if(debounce_ms < -1 || debounce_ms==0 || debounce_ms>5)
        return fail(e,en,"Debounce must be from 1 through 5 milliseconds");
    if(sleep_seconds!=-1 && model==AULA_MODEL_F87)
        return fail(e,en,"F87 has no confirmed sleep mapping in the shipped configuration");
    for(size_t i=0;i<sizeof(sleep_values)/sizeof(*sleep_values);i++)
        if(sleep_seconds==sleep_values[i]) sleep_ok=1;
    if(!sleep_ok) return fail(e,en,"Unsupported sleep timeout");
    if(debounce_ms!=-1) cfg[3]=(unsigned char)(debounce_ms-1);
    if(sleep_seconds!=-1) cfg[0x18]=(unsigned char)(sleep_seconds/30);
    return 0;
}
