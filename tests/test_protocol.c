#include "../src/protocol.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static unsigned char stored[AULA_CFG_SIZE];
static unsigned char expected_header[8]={6,0x84,0,0,1,0,128,0};
static unsigned sets, gets;
static int bad_header, short_reply, mismatch;
static int response_length=520;
int aula_hid_set_feature(aula_hid_device *d,unsigned id,unsigned char *b,size_t n,char *e,size_t en) {
    (void)d; (void)e; (void)en;
    assert(id==6 && n==520 && b[0]==6 && b[4]==1 && b[5]==0 && b[6]==128 && b[7]==0);
    assert(b[1]==4 || b[1]==0x84); sets++;
    if(b[1]==4) memcpy(stored,b+8,sizeof(stored));
    return (int)n;
}
int aula_hid_get_feature(aula_hid_device *d,unsigned id,unsigned char *b,size_t n,char *e,size_t en) {
    (void)d; (void)e; (void)en; assert(id==6 && n==520); gets++;
    memset(b,0,n); memcpy(b,expected_header,8); memcpy(b+8,stored,sizeof(stored));
    if(bad_header) b[5]=1;
    if(mismatch) b[8]^=1;
    return short_reply ? 135 : response_length;
}
static void fixture(unsigned char cfg[128]) {
    for(unsigned i=0;i<128;i++) cfg[i]=(unsigned char)(i*13+7);
    cfg[10]=5; cfg[126]=0x5a; cfg[127]=0xa5;
}
int main(void) {
    unsigned char cfg[128],before[128],packet[520],output[128]; char error[256];
    aula_hid_device dev;
    memset(&dev,0,sizeof(dev)); dev.fd=123; dev.writable=1;
    dev.vendor=0x258a; dev.product=0x010c;
    dev.reports.feature[6].present=1; dev.reports.feature[6].ioctl_bytes=520;
    /* Captured from the wired keyboard: 136 bytes, including the header. */
    {
        unsigned char captured[136]; unsigned value;
        FILE *f=fopen("tests/fixtures/wired-config-read.hex","r"); assert(f);
        for(unsigned i=0;i<sizeof(captured);i++) {
            assert(fscanf(f,"%2x",&value)==1); captured[i]=(unsigned char)value;
        }
        fclose(f);
        assert(!memcmp(captured,expected_header,8));
        memcpy(stored,captured+8,128); response_length=136;
        assert(!aula_protocol_read_cfg(&dev,output,error,sizeof(error)));
        assert(!memcmp(output,captured+8,128));
        assert(output[10]==6 && output[0x44]==0 && output[0x45]>>4==4);
        response_length=520; sets=gets=0;
    }
    fixture(cfg); memcpy(before,cfg,128);
    assert(!aula_protocol_patch_cfg(cfg,AULA_MODEL_F75,18,9,4,error,sizeof(error)));
    for(unsigned i=0;i<128;i++) {
        unsigned char expected=before[i];
        if(i==9) expected=0;
        if(i==10) expected=18;
        if(i==0x38 || i==0x39) expected=0xff;
        if(i==0x5c) expected=9;
        if(i==0x5d) expected=(unsigned char)((before[i]&15)|0x40);
        assert(cfg[i]==expected);
    }
    fixture(cfg); memcpy(before,cfg,128);
    assert(!aula_protocol_patch_cfg(cfg,AULA_MODEL_F99,-1,-1,-1,error,sizeof(error)));
    assert(!memcmp(cfg,before,128));
    assert(aula_protocol_patch_cfg(cfg,AULA_MODEL_F99,1,5,0,error,sizeof(error))<0);
    assert(!memcmp(cfg,before,128));
    assert(aula_protocol_patch_cfg(cfg,AULA_MODEL_F75,19,0,0,error,sizeof(error))<0);
    assert(!memcmp(cfg,before,128));
    assert(!aula_protocol_patch_cfg(cfg,AULA_MODEL_F87,0,4,4,error,sizeof(error)));
    for(unsigned i=0x3a;i<128;i++) assert(cfg[i]==before[i]);
    fixture(cfg); cfg[10]=21; memcpy(before,cfg,128);
    assert(aula_protocol_patch_cfg(cfg,AULA_MODEL_F75,-1,1,-1,error,sizeof(error))<0);
    assert(!memcmp(cfg,before,128));
    assert(!aula_protocol_patch_cfg(cfg,AULA_MODEL_F75,1,-1,-1,error,sizeof(error)));
    fixture(cfg); cfg[126]=0; memcpy(before,cfg,128);
    assert(aula_protocol_patch_cfg(cfg,AULA_MODEL_F75,1,1,1,error,sizeof(error))<0);
    assert(!memcmp(cfg,before,128));
    assert(!aula_protocol_build_packet(0x84,NULL,packet,error,sizeof(error)));
    assert(!memcmp(packet,expected_header,8));
    for(unsigned i=8;i<520;i++) assert(!packet[i]);
    fixture(cfg);
    assert(!aula_protocol_build_packet(4,cfg,packet,error,sizeof(error)));
    assert(packet[1]==4 && !memcmp(packet+8,cfg,128));
    for(unsigned i=136;i<520;i++) assert(!packet[i]);
    assert(aula_protocol_build_packet(4,NULL,packet,error,sizeof(error))<0);
    memcpy(stored,cfg,128);
    assert(!aula_protocol_read_cfg(&dev,output,error,sizeof(error)));
    assert(!memcmp(output,cfg,128) && sets==1 && gets==1);
    response_length=136;
    assert(!aula_protocol_read_cfg(&dev,output,error,sizeof(error)));
    assert(!memcmp(output,cfg,128));
    response_length=519;
    assert(!aula_protocol_read_cfg(&dev,output,error,sizeof(error)));
    response_length=520;
    bad_header=1; assert(aula_protocol_read_cfg(&dev,output,error,sizeof(error))<0); bad_header=0;
    short_reply=1; assert(aula_protocol_read_cfg(&dev,output,error,sizeof(error))<0); short_reply=0;
    stored[127]=0; assert(aula_protocol_read_cfg(&dev,output,error,sizeof(error))<0);
    memcpy(stored,cfg,128);
    assert(!aula_protocol_write_cfg(&dev,cfg,error,sizeof(error)));
    mismatch=1; assert(aula_protocol_write_cfg(&dev,cfg,error,sizeof(error))<0); mismatch=0;
    dev.vendor=0x3554; dev.product=0xfa09;
    { unsigned oldsets=sets;
      assert(aula_protocol_read_cfg(&dev,output,error,sizeof(error))<0);
      assert(aula_protocol_write_cfg(&dev,cfg,error,sizeof(error))<0);
      assert(sets==oldsets);
    }
    puts("Wired protocol preservation, packets, validation and read-back fixtures passed");
    return 0;
}
