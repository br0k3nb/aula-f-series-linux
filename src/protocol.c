#define _POSIX_C_SOURCE 200809L
#include "protocol.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
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
int aula_protocol_build_packet(unsigned cmd,const unsigned char *payload,
                               unsigned char packet[AULA_PACKET_SIZE],char *e,size_t en) {
    if(!packet || (cmd!=0x84 && cmd!=0x04) || (cmd==0x04 && !payload))
        return fail(e,en,"Invalid G5 configuration command or payload");
    memset(packet,0,AULA_PACKET_SIZE);
    packet[0]=AULA_FEATURE_ID; packet[1]=(unsigned char)cmd;
    packet[4]=1; packet[6]=AULA_CFG_SIZE;
    if(payload) memcpy(packet+8,payload,AULA_CFG_SIZE);
    return 0;
}
int aula_protocol_read_cfg(aula_hid_device *d,unsigned char cfg[AULA_CFG_SIZE],char *e,size_t en) {
    unsigned char request[AULA_PACKET_SIZE],reply[AULA_PACKET_SIZE]; int n;
    if(!cfg) return fail(e,en,"Missing configuration output buffer");
    if(check_device(d,e,en)<0 || aula_protocol_build_packet(0x84,NULL,request,e,en)<0) return -1;
    if(pause20(e,en)<0) return -1;
    n=aula_hid_set_feature(d,AULA_FEATURE_ID,request,sizeof(request),e,en);
    if(n<0) return -1;
    if(n!=(int)sizeof(request)) return fail(e,en,"Short configuration request write: %d bytes",n);
    if(pause20(e,en)<0) return -1;
    memset(reply,0,sizeof(reply));
    n=aula_hid_get_feature(d,AULA_FEATURE_ID,reply,sizeof(reply),e,en);
    if(n<0) return -1;
    /* Linux returns the actual transfer length. Observed wired firmware sends
     * the header plus 128 config bytes (136), despite declaring a 520-byte
     * feature report. Both that response and a fully padded report are valid. */
    if(n<(int)(8+AULA_CFG_SIZE) || n>(int)sizeof(reply))
        return fail(e,en,"Incomplete configuration response: %d bytes (need at least 136)",n);
    if(memcmp(reply,request,8))
        return fail(e,en,"Unexpected configuration response header; physical-device validation required");
    if(check_cfg(reply+8,e,en)<0) return -1;
    memcpy(cfg,reply+8,AULA_CFG_SIZE);
    return 0;
}
int aula_protocol_patch_cfg(unsigned char cfg[AULA_CFG_SIZE],enum aula_model model,
                            int mode,int brightness,int speed,char *e,size_t en) {
    int selected, max_brightness;
    if(check_cfg(cfg,e,en)<0) return -1;
    if(model!=AULA_MODEL_F75 && model!=AULA_MODEL_F87 && model!=AULA_MODEL_F99)
        return fail(e,en,"Unknown keyboard model");
    max_brightness=model==AULA_MODEL_F75 ? 9 : 4;
    if(mode < -1 || mode > 18 || brightness < -1 || brightness > max_brightness || speed < -1 || speed > 4)
        return fail(e,en,"Invalid lighting setting for this model");
    if(mode==-1 && brightness==-1 && speed==-1) return 0;
    selected=mode==-1 ? cfg[10] : mode;
    if(selected<0 || selected>18)
        return fail(e,en,"Current hardware mode is unsupported; select an explicit mode 0..18");
    cfg[9]=0; cfg[10]=(unsigned char)selected; cfg[0x38]=0xff; cfg[0x39]=0xff;
    if(selected) {
        size_t offset=0x3a+2u*(unsigned)(selected-1);
        if(brightness!=-1) cfg[offset]=(unsigned char)brightness;
        if(speed!=-1) cfg[offset+1]=(unsigned char)((cfg[offset+1]&0x0f)|((unsigned)speed<<4));
    }
    return 0;
}
int aula_protocol_write_cfg(aula_hid_device *d,const unsigned char cfg[AULA_CFG_SIZE],char *e,size_t en) {
    unsigned char packet[AULA_PACKET_SIZE],verify[AULA_CFG_SIZE]; int n;
    if(check_device(d,e,en)<0 || check_cfg(cfg,e,en)<0 ||
       aula_protocol_build_packet(4,cfg,packet,e,en)<0) return -1;
    if(pause20(e,en)<0) return -1;
    n=aula_hid_set_feature(d,AULA_FEATURE_ID,packet,sizeof(packet),e,en);
    if(n<0) return -1;
    if(n!=(int)sizeof(packet)) return fail(e,en,"Short configuration write: %d bytes",n);
    if(aula_protocol_read_cfg(d,verify,e,en)<0) return -1;
    if(memcmp(cfg,verify,AULA_CFG_SIZE))
        return fail(e,en,"Configuration read-back differs; device may already have applied part of the change");
    return 0;
}
