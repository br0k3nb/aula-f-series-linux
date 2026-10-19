#define _POSIX_C_SOURCE 200809L
#include "keymap_cli.h"
#include "keymap.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
static unsigned char maps[4][512],pool[4096];
static unsigned opens,writes;
int aula_hid_open(const char *node,int writable,aula_hid_device *d,char *e,size_t n) {
    (void)e;(void)n;assert(!strcmp(node,"/dev/hidraw-test")&&writable==1);memset(d,0,sizeof(*d));opens++;return 0;
}
void aula_hid_close(aula_hid_device *d) {(void)d;}
int aula_g5_read_data(aula_hid_device *d,unsigned cmd,unsigned sub,unsigned char *out,size_t len,char *e,size_t n) {
    (void)d;(void)e;(void)n;
    if(cmd==0x83){assert(sub<4&&len==512);memcpy(out,maps[sub],512);}
    else{assert(cmd==0x85&&sub==0&&len==4096);memcpy(out,pool,4096);}
    return 0;
}
int aula_g5_write_data(aula_hid_device *d,unsigned cmd,unsigned sub,const unsigned char *in,size_t len,char *e,size_t n) {
    (void)d;(void)e;(void)n;writes++;
    if(cmd==3){assert(sub<4&&len==512);memcpy(maps[sub],in,512);}
    else{assert(cmd==5&&sub==0&&len==4096);memcpy(pool,in,4096);}
    return 0;
}
#define RUN(...) do{char *args[]={"aula-control",__VA_ARGS__};assert(aula_keymap_cli(sizeof(args)/sizeof(args[0]),args)==0);}while(0)
#define BAD(expected,...) do{char *args[]={"aula-control",__VA_ARGS__};assert(aula_keymap_cli(sizeof(args)/sizeof(args[0]),args)==expected);}while(0)
int main(void) {
    assert(freopen("/dev/null","w",stdout));
    for(unsigned i=0;i<4;i++){maps[i][510]=0x5a;maps[i][511]=0xa5;}
    RUN("--keymap","/dev/hidraw-test","--layer","2");
    RUN("--remap","/dev/hidraw-test","--layer","2","--key","20","--action","combo","--code","0x04","--modifiers","3","--code2","5");
    assert(!memcmp(maps[2]+80,"\0\3\5\4",4));
    unsigned prev_opens=opens,prev_writes=writes;
    BAD(2,"--remap","/dev/hidraw-test","--key","127","--action","key","--code","4");
    BAD(2,"--remap","/dev/hidraw-test","--key","20","--action","special","--code","0x12345678");
    BAD(2,"--keymap","/dev/hidraw-test","--layer","2","--layer","1");
    assert(opens==prev_opens&&writes==prev_writes);
    RUN("--remap","/dev/hidraw-test","--key","10","--action","media","--code","0x183");
    assert(!memcmp(maps[0]+40,"\2\0\1\203",4));
    char path[]="/tmp/aula-macro-test-XXXXXX";int fd=mkstemp(path);assert(fd>=0);
    FILE *f=fdopen(fd,"w");assert(f);fputs("0,4,1,0\n0,4,0,100\n",f);fclose(f);
    RUN("--macro-add","/dev/hidraw-test","--name","Example","--events",path,"--layer","1","--key","4","--repeat","2","--trigger","once");
    assert(!memcmp(maps[1]+16,"\3\1\2\0",4));
    unsigned count;size_t used;char error[256];assert(!aula_macro_count(pool,&count,&used,error,sizeof(error))&&count==1);
    RUN("--macro-add","/dev/hidraw-test","--name","Other","--events",path,"--layer","2","--key","8");
    assert(!aula_macro_count(pool,&count,&used,error,sizeof(error))&&count==2);
    unsigned second_offset=pool[4]|(unsigned)pool[5]<<8,second_len=pool[6]|(unsigned)pool[7]<<8;
    unsigned char second[4096],map_before[4][512],pool_before[4096];
    memcpy(second,pool+second_offset,second_len);memcpy(map_before,maps,sizeof(maps));
    f=fopen(path,"w");assert(f);fputs("0,5,1,0\n0,5,0,70\n",f);fclose(f);
    RUN("--macro-update","/dev/hidraw-test","--index","0","--name","Edited macro","--events",path);
    assert(!aula_macro_count(pool,&count,&used,error,sizeof(error))&&count==2);
    assert(!memcmp(map_before,maps,sizeof(maps)));
    second_offset=pool[4]|(unsigned)pool[5]<<8;
    assert(second_len==(pool[6]|(unsigned)pool[7]<<8)&&!memcmp(second,pool+second_offset,second_len));
    unsigned first_offset=pool[0]|(unsigned)pool[1]<<8;
    assert(pool[first_offset]==24&&pool[first_offset+25+3]==5);
    memcpy(pool_before,pool,sizeof(pool));prev_writes=writes;
    BAD(1,"--macro-update","/dev/hidraw-test","--index","99","--name","Missing","--events",path);
    assert(writes==prev_writes&&!memcmp(pool_before,pool,sizeof(pool))&&!memcmp(map_before,maps,sizeof(maps)));
    f=fopen(path,"w");assert(f);for(unsigned i=0;i<1008;i++)fputs("0,4,1,0\n",f);fclose(f);
    BAD(1,"--macro-update","/dev/hidraw-test","--index","0","--name","123456789012345678901234567890","--events",path);
    assert(writes==prev_writes&&!memcmp(pool_before,pool,sizeof(pool))&&!memcmp(map_before,maps,sizeof(maps)));
    RUN("--remap","/dev/hidraw-test","--layer","2","--key","8","--action","disable");
    RUN("--macro-delete","/dev/hidraw-test","--index","1");
    RUN("--macro-list","/dev/hidraw-test");
    RUN("--remap","/dev/hidraw-test","--layer","3","--key","20","--action","macro","--code","0","--trigger","hold");
    assert(!memcmp(maps[3]+80,"\3\2\1\0",4));
    prev_writes=writes;
    BAD(1,"--macro-delete","/dev/hidraw-test","--index","0");assert(writes==prev_writes);
    RUN("--remap","/dev/hidraw-test","--layer","1","--key","4","--action","disable");
    RUN("--remap","/dev/hidraw-test","--layer","3","--key","20","--action","disable");
    RUN("--macro-delete","/dev/hidraw-test","--index","0");
    assert(!aula_macro_count(pool,&count,&used,error,sizeof(error))&&count==0);
    unlink(path);fputs("keymap CLI tests passed\n",stderr);return 0;
}
