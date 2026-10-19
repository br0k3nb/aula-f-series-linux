#include "keymap.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned char maps[4][512],pool[4096];
static int mismatch,bad_marker;
static unsigned reads,writes;
int aula_g5_read_data(aula_hid_device *d,unsigned cmd,unsigned sub,unsigned char *out,size_t len,char *e,size_t n) {
    (void)d;(void)e;(void)n;reads++;
    if(cmd==0x83) {assert(sub<4&&len==512);memcpy(out,maps[sub],512);if(bad_marker)out[511]=0;}
    else {assert(cmd==0x85&&sub==0&&len==4096);memcpy(out,pool,4096);}
    if(mismatch) out[len-3]^=1;
    return 0;
}
int aula_g5_write_data(aula_hid_device *d,unsigned cmd,unsigned sub,const unsigned char *in,size_t len,char *e,size_t n) {
    (void)d;(void)e;(void)n;writes++;
    if(cmd==3) {assert(sub<4&&len==512);memcpy(maps[sub],in,len);}
    else {assert(cmd==5&&sub==0&&len==4096);memcpy(pool,in,len);}
    return 0;
}
int main(void) {
    char error[256];unsigned char map[512],original[512],a[4],blob[4096],blob2[4096],p[4096]={0},before[4096];
    size_t len,len2,used;unsigned total,index;aula_hid_device device={0};
    memset(map,0xaa,512);map[510]=0x5a;map[511]=0xa5;memcpy(original,map,512);
    assert(!aula_keymap_key(3,4,5,a,error,sizeof(error))&&!memcmp(a,"\0\3\5\4",4));
    assert(!aula_keymap_patch(map,20,a,error,sizeof(error)));
    assert(!memcmp(map,original,80)&&!memcmp(map+84,original+84,512-84));
    memcpy(original,map,512);assert(aula_keymap_patch(map,127,a,error,sizeof(error))==-1&&!memcmp(map,original,512));
    assert(aula_keymap_key(256,4,0,a,error,sizeof(error))==-1);
    assert(!aula_keymap_media(0x183,a,error,sizeof(error))&&!memcmp(a,"\2\0\1\203",4));
    assert(!aula_keymap_mouse(0x12,a,error,sizeof(error))&&!memcmp(a,"\1\3\1\0",4));
    assert(!aula_keymap_mouse(0x16,a,error,sizeof(error))&&!memcmp(a,"\1\1\2\62",4));
    aula_keymap_raw(0x07000001,a);assert(!memcmp(a,"\7\0\0\1",4));
    assert(!aula_keymap_macro(4,2,100,a,error,sizeof(error))&&!memcmp(a,"\3\2\1\4",4));
    aula_macro_event events[]={{0,0,4,1},{0,100,4,0},{1,0xfffff,0xe1,1},{3,1,-10,0}};
    assert(!aula_macro_pack("A\xf0\x9f\x98\x80",events,4,blob,sizeof(blob),&len,error,sizeof(error)));
    assert(len==23&&blob[0]==6&&!memcmp(blob+1,"A\0\75\330\0\336",6));
    assert(!memcmp(blob+7,"\0\0\0\4\200\0\144\4\37\377\377\341\60\0\1\366",16));
    assert(!aula_macro_pack("second",events,2,blob2,sizeof(blob2),&len2,error,sizeof(error)));
    assert(!aula_macro_append(p,blob,len,&index,error,sizeof(error))&&index==0);
    assert(!aula_macro_count(p,&total,&used,error,sizeof(error))&&total==1&&used==len+4);
    assert(!memcmp(p+4,blob,len));
    assert(!aula_macro_append(p,blob2,len2,&index,error,sizeof(error))&&index==1);
    assert(!aula_macro_count(p,&total,&used,error,sizeof(error))&&total==2&&used==len+len2+8);
    assert(!memcmp(p+8,blob,len)&&!memcmp(p+8+len,blob2,len2));
    p[4]=0xff;memcpy(before,p,4096);
    assert(aula_macro_append(p,blob,len,&index,error,sizeof(error))==-1&&!memcmp(p,before,4096));
    assert(aula_macro_pack("bad\xed\xa0\x80",events,1,blob,sizeof(blob),&len,error,sizeof(error))==-1);
    events[0].delay=0x100000;assert(aula_macro_pack_event(events,a,error,sizeof(error))==-1);events[0].delay=0;
    memcpy(maps[2],map,512);assert(!aula_keymap_read(&device,2,original,error,sizeof(error)));
    bad_marker=1;memset(original,0x55,512);assert(aula_keymap_read(&device,2,original,error,sizeof(error))==-1&&original[0]==0x55);bad_marker=0;
    assert(!aula_keymap_write(&device,2,map,error,sizeof(error)));
    mismatch=1;assert(aula_keymap_write(&device,2,map,error,sizeof(error))==-1);mismatch=0;
    memset(p,0,4096);assert(!aula_macro_append(p,blob2,len2,&index,error,sizeof(error)));
    assert(!aula_macro_write(&device,p,error,sizeof(error)));
    assert(!aula_macro_read(&device,before,error,sizeof(error))&&!memcmp(before,p,4096));
    mismatch=1;assert(aula_macro_write(&device,p,error,sizeof(error))==-1);
    assert(reads>=7&&writes==4);
    puts("keymap and macro tests passed");return 0;
}
