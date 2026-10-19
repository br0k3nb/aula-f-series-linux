#include "lighting_ext.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static unsigned char stored[0x2800],header[8];
static unsigned sets,gets;
static int short_reply,bad_header,wrong_data,short_write;
static int fail_chunk=-1;
int aula_hid_set_feature(aula_hid_device *d,unsigned id,unsigned char *b,size_t n,char *e,size_t en) {
    size_t length=(size_t)b[6]+((size_t)b[7]<<8);
    size_t offset=(size_t)b[5]*512;
    (void)d; (void)e; (void)en;
    assert(id==6 && n==520 && b[0]==6 && b[3]==0 && b[4]>0 && b[5]<b[4]);
    assert(length>0 && length<=512 && offset+length<=sizeof(stored));
    sets++; memcpy(header,b,8);
    if(!(b[1]&0x80)) memcpy(stored+offset,b+8,length);
    for(size_t i=8+length;i<n;i++) assert(b[i]==0);
    if((int)b[5]==fail_chunk) return -1;
    return short_write ? 519 : (int)n;
}
int aula_hid_get_feature(aula_hid_device *d,unsigned id,unsigned char *b,size_t n,char *e,size_t en) {
    size_t length=(size_t)header[6]+((size_t)header[7]<<8);
    size_t offset=(size_t)header[5]*512;
    (void)d; (void)e; (void)en;
    assert(id==6 && n==520 && (header[1]&0x80)); gets++;
    memset(b,0,n); memcpy(b,header,8); memcpy(b+8,stored+offset,length);
    if(bad_header) b[5]^=1;
    if(wrong_data) b[8]^=1;
    return short_reply ? (int)(7+length) : (int)(8+length);
}
static void fixture(unsigned char *data,size_t n) {
    for(size_t i=0;i<n;i++) data[i]=(unsigned char)(i*23+11);
}
int main(void) {
    aula_hid_device dev;
    unsigned char rgb[512],original[512],cfg[128],before[128],packet[520],palette[21];
    unsigned char large[0x2800],output[0x2800];
    unsigned char perkey[AULA_PERKEY_SIZE],original_perkey[AULA_PERKEY_SIZE];
    char error[256];
    memset(&dev,0,sizeof(dev)); dev.fd=4; dev.writable=1;
    dev.vendor=0x258a; dev.product=0x010c;
    dev.reports.feature[6].present=1; dev.reports.feature[6].ioctl_bytes=520;
    { /* Live F75 palette has its marker at506; keymap marker is510. */
      unsigned value; FILE *f=fopen("tests/fixtures/wired-rgb-read.hex","r");assert(f);
      for(unsigned i=0;i<512;i++){assert(fscanf(f,"%2x",&value)==1);stored[i]=(unsigned char)value;}
      fclose(f);
      assert(stored[506]==0x5a&&stored[507]==0xa5&&stored[510]==0);
      assert(!aula_lighting_read_rgb(&dev,output,error,sizeof(error)));
      assert(!memcmp(output,stored,512));
      assert(!aula_lighting_patch_palette(output,1,0,128,64,255,error,sizeof(error)));
      assert(output[21]==128&&output[22]==64&&output[23]==255&&output[506]==0x5a);
      sets=gets=0;
    }

    assert(!aula_g5_build_packet(0x8a,0,1,0,NULL,512,packet,error,sizeof(error)));
    assert(!memcmp(packet,(unsigned char[]){6,0x8a,0,0,1,0,0,2},8));
    for(size_t i=8;i<520;i++) assert(packet[i]==0);
    assert(!aula_g5_build_packet(5,2,3,2,(unsigned char[]){3,2,1},3,packet,error,sizeof(error)));
    assert(!memcmp(packet,(unsigned char[]){6,5,2,0,3,2,3,0,3,2,1},11));
    assert(aula_g5_build_packet(5,0,1,1,NULL,512,packet,error,sizeof(error))<0);
    assert(aula_g5_build_packet(5,0,256,1,NULL,512,packet,error,sizeof(error))<0);
    assert(aula_g5_build_packet(5,0,1,0,NULL,513,packet,error,sizeof(error))<0);

    fixture(large,sizeof(large));
    assert(!aula_g5_write_data(&dev,5,3,large,sizeof(large),error,sizeof(error)));
    assert(sets==20 && !memcmp(stored,large,sizeof(large)));
    assert(header[4]==20 && header[5]==19 && header[6]==0 && header[7]==2);
    assert(!aula_g5_read_data(&dev,0x85,3,output,sizeof(output),error,sizeof(error)));
    assert(!memcmp(output,large,sizeof(output)) && gets==20);
    sets=gets=0;
    assert(!aula_g5_write_data(&dev,5,2,large,1025,error,sizeof(error)));
    assert(header[4]==3 && header[5]==2 && header[6]==1 && header[7]==0);
    assert(!aula_g5_read_data(&dev,0x85,2,output,1025,error,sizeof(error)));
    assert(!memcmp(output,large,1025) && sets==6 && gets==3);
    memset(output,0xaa,sizeof(output)); fail_chunk=1;
    assert(aula_g5_read_data(&dev,0x85,2,output,1025,error,sizeof(error))<0);
    for(size_t i=0;i<sizeof(output);i++) assert(output[i]==0xaa);
    fail_chunk=-1; short_reply=1;
    assert(aula_g5_read_data(&dev,0x85,2,output,1,error,sizeof(error))<0);
    short_reply=0; bad_header=1;
    assert(aula_g5_read_data(&dev,0x85,2,output,1,error,sizeof(error))<0);
    bad_header=0; short_write=1;
    assert(aula_g5_write_data(&dev,5,0,large,1,error,sizeof(error))<0);
    short_write=0;
    { unsigned old_sets=sets;
      assert(aula_g5_write_data(&dev,0x85,0,large,1,error,sizeof(error))<0);
      assert(aula_g5_read_data(&dev,5,0,output,1,error,sizeof(error))<0);
      assert(aula_g5_read_data(&dev,0x85,0,output,AULA_G5_MAX_DATA+1,error,sizeof(error))<0);
      dev.writable=0;
      assert(aula_g5_read_data(&dev,0x85,0,output,1,error,sizeof(error))<0);
      dev.writable=1; dev.vendor=0;
      assert(aula_g5_write_data(&dev,5,0,large,1,error,sizeof(error))<0);
      dev.vendor=0x258a; assert(sets==old_sets);
    }

    fixture(rgb,sizeof(rgb)); rgb[510]=0x5a; rgb[511]=0xa5; memcpy(original,rgb,sizeof(rgb));
    assert(!aula_lighting_patch_palette(rgb,1,0,20,40,60,error,sizeof(error)));
    for(size_t i=0;i<sizeof(rgb);i++) {
        unsigned char expected=original[i];
        if(i==21) expected=20;
        if(i==22) expected=40;
        if(i==23) expected=60;
        assert(rgb[i]==expected);
    }
    memcpy(rgb,original,sizeof(rgb)); fixture(palette,sizeof(palette));
    assert(!aula_lighting_patch_colors(rgb,18,palette,error,sizeof(error)));
    assert(!memcmp(rgb+18*21,palette,21));
    assert(!memcmp(rgb,original,18*21) && !memcmp(rgb+19*21,original+19*21,512-19*21));
    memcpy(rgb,original,sizeof(rgb));
    assert(aula_lighting_patch_palette(rgb,19,0,0,0,0,error,sizeof(error))<0);
    assert(aula_lighting_patch_palette(rgb,1,7,0,0,0,error,sizeof(error))<0);
    assert(aula_lighting_patch_palette(rgb,1,0,256,0,0,error,sizeof(error))<0);
    assert(!memcmp(rgb,original,sizeof(rgb)));
    memcpy(stored,rgb,512);
    assert(!aula_lighting_read_rgb(&dev,output,error,sizeof(error)));
    assert(!memcmp(output,rgb,512));
    assert(!aula_lighting_write_rgb(&dev,rgb,error,sizeof(error)));
    wrong_data=1;
    assert(aula_lighting_write_rgb(&dev,rgb,error,sizeof(error))<0); wrong_data=0;
    stored[511]=0; memset(output,0xaa,512);
    assert(aula_lighting_read_rgb(&dev,output,error,sizeof(error))<0);
    for(size_t i=0;i<512;i++) assert(output[i]==0xaa);

    fixture(perkey,sizeof(perkey)); memcpy(original_perkey,perkey,sizeof(perkey));
    assert(!aula_lighting_patch_perkey(perkey,125,12,34,56,error,sizeof(error)));
    for(size_t i=0;i<sizeof(perkey);i++) {
        unsigned char expected=original_perkey[i];
        if(i==125) expected=12;
        if(i==251) expected=34;
        if(i==377) expected=56;
        assert(perkey[i]==expected);
    }
    memcpy(perkey,original_perkey,sizeof(perkey));
    assert(aula_lighting_patch_perkey(perkey,126,1,2,3,error,sizeof(error))<0);
    assert(aula_lighting_patch_perkey(perkey,0,1,2,256,error,sizeof(error))<0);
    assert(!memcmp(perkey,original_perkey,sizeof(perkey)));
    assert(!aula_lighting_patch_perkey(perkey,0,255,0,127,error,sizeof(error)));
    assert(perkey[0]==255 && perkey[126]==0 && perkey[252]==127);
    assert(!aula_lighting_write_perkey(&dev,perkey,error,sizeof(error)));
    assert(header[1]==0x86 && header[2]==0 && header[6]==0x7a && header[7]==1);
    assert(!aula_lighting_read_perkey(&dev,output,error,sizeof(error)));
    assert(!memcmp(perkey,output,sizeof(perkey)));
    wrong_data=1;
    assert(aula_lighting_write_perkey(&dev,perkey,error,sizeof(error))<0);
    wrong_data=0;
    { unsigned old_gets=gets,old_sets=sets;
      assert(!aula_lighting_send_stream(&dev,perkey,error,sizeof(error)));
      assert(header[1]==8 && header[2]==0 && header[6]==0x7a && header[7]==1);
      assert(gets==old_gets && sets==old_sets+1 && !memcmp(perkey,stored,sizeof(perkey)));
    }

    fixture(cfg,sizeof(cfg)); cfg[126]=0x5a; cfg[127]=0xa5; memcpy(before,cfg,sizeof(cfg));
    assert(!aula_lighting_patch_color_selection(cfg,18,7,error,sizeof(error)));
    for(size_t i=0;i<sizeof(cfg);i++)
        assert(cfg[i]==(i==0x5d ? (unsigned char)((before[i]&0xf0)|7) : before[i]));
    memcpy(cfg,before,sizeof(cfg));
    assert(!aula_lighting_patch_settings(cfg,AULA_MODEL_F75,4,900,error,sizeof(error)));
    for(size_t i=0;i<sizeof(cfg);i++)
        assert(cfg[i]==(i==3 ? 3 : i==0x18 ? 30 : before[i]));
    memcpy(cfg,before,sizeof(cfg));
    assert(aula_lighting_patch_settings(cfg,AULA_MODEL_F75,1,75,error,sizeof(error))<0);
    assert(aula_lighting_patch_settings(cfg,AULA_MODEL_F75,6,60,error,sizeof(error))<0);
    assert(aula_lighting_patch_settings(cfg,AULA_MODEL_F87,1,60,error,sizeof(error))<0);
    assert(!memcmp(cfg,before,sizeof(cfg)));
    assert(!aula_lighting_patch_settings(cfg,AULA_MODEL_F87,1,-1,error,sizeof(error)));
    assert(cfg[3]==0 && cfg[0x18]==before[0x18]);
    assert(!aula_lighting_patch_settings(cfg,AULA_MODEL_F75,5,-1,error,sizeof(error)));
    assert(cfg[3]==4);
    memcpy(cfg,before,sizeof(cfg));
    assert(!aula_lighting_select_custom(cfg,error,sizeof(error)));
    for(size_t i=0;i<sizeof(cfg);i++)
        assert(cfg[i]==(i==9 ? 1 : i==10 ? 21 : before[i]));
    memcpy(cfg,before,sizeof(cfg));
    assert(!aula_lighting_patch_indicator(cfg,0,error,sizeof(error)));
    for(size_t i=0;i<sizeof(cfg);i++) assert(cfg[i]==(i==0x12 ? 0 : before[i]));
    assert(!aula_lighting_patch_indicator(cfg,1,error,sizeof(error)) && cfg[0x12]==1);
    memcpy(before,cfg,sizeof(cfg));
    assert(!aula_lighting_patch_indicator(cfg,-1,error,sizeof(error)));
    assert(aula_lighting_patch_indicator(cfg,2,error,sizeof(error))<0);
    assert(!memcmp(cfg,before,sizeof(cfg)));
    cfg[127]=0; memcpy(before,cfg,sizeof(cfg));
    assert(aula_lighting_patch_indicator(cfg,1,error,sizeof(error))<0);
    assert(aula_lighting_select_custom(cfg,error,sizeof(error))<0);
    assert(!memcmp(cfg,before,sizeof(cfg)));
    puts("Extended G5 chunks, RGB preservation/read-back, palette and settings tests passed");
    return 0;
}
