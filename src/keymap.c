#define _POSIX_C_SOURCE 200809L
#include "keymap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int fail(char *error, size_t n, const char *message) {
    if (error && n) snprintf(error,n,"%s",message);
    return -1;
}
static void wait_written(void) {
    struct timespec delay={0,100000000};
    while (nanosleep(&delay,&delay)) { /* A signal may interrupt the delay. */ }
}
static unsigned le16(const unsigned char *p) { return p[0]|(unsigned)p[1]<<8; }
static void put16(unsigned char *p,unsigned v) { p[0]=(unsigned char)v;p[1]=(unsigned char)(v>>8); }
static int valid_map(const unsigned char *p,char *e,size_t n) {
    if (!p || p[510]!=0x5a || p[511]!=0xa5) return fail(e,n,"Invalid keymap marker; keyboard data was not changed.");
    return 0;
}
int aula_keymap_read(aula_hid_device *d,unsigned layer,unsigned char map[512],char *e,size_t n) {
    unsigned char temp[512];
    if (layer>=4 || !map) return fail(e,n,"Keymap layer must be0..3.");
    if (aula_g5_read_data(d,0x83,layer,temp,512,e,n) || valid_map(temp,e,n)) return -1;
    memcpy(map,temp,512);return 0;
}
int aula_keymap_write(aula_hid_device *d,unsigned layer,const unsigned char map[512],char *e,size_t n) {
    unsigned char actual[512];
    if (layer>=4) return fail(e,n,"Keymap layer must be0..3.");
    if (valid_map(map,e,n)) return -1;
    if (aula_g5_write_data(d,3,layer,map,512,e,n)) return -1;
    wait_written();
    if (aula_keymap_read(d,layer,actual,e,n)) return -1;
    if (memcmp(actual,map,512)) return fail(e,n,"Keymap write verification failed; keyboard may already have changed.");
    return 0;
}
int aula_keymap_patch(unsigned char map[512],unsigned index,const unsigned char a[4],char *e,size_t n) {
    if (index>=127 || !a) return fail(e,n,"Physical key index must be0..126.");
    if (valid_map(map,e,n)) return -1;
    memcpy(map+index*4,a,4);return 0;
}
int aula_keymap_key(unsigned modifiers,unsigned key1,unsigned key2,unsigned char a[4],char *e,size_t n) {
    if (!a || modifiers>255 || key1>255 || key2>255) return fail(e,n,"Keyboard usages and modifiers must fit in one byte.");
    a[0]=0;a[1]=(unsigned char)modifiers;a[2]=(unsigned char)key2;a[3]=(unsigned char)key1;return 0;
}
int aula_keymap_media(unsigned usage,unsigned char a[4],char *e,size_t n) {
    if (!a || !usage || usage>65535) return fail(e,n,"Consumer usage must be1..65535.");
    a[0]=2;a[1]=0;a[2]=(unsigned char)(usage>>8);a[3]=(unsigned char)usage;return 0;
}
void aula_keymap_raw(uint32_t code,unsigned char a[4]) {
    a[0]=(unsigned char)(code>>24);a[1]=(unsigned char)(code>>16);a[2]=(unsigned char)(code>>8);a[3]=(unsigned char)code;
}
int aula_keymap_mouse(unsigned action,unsigned char a[4],char *e,size_t n) {
    uint32_t raw;
    switch(action) {
    case 0x11:raw=0x01010100;break;case 0x12:raw=0x01030100;break;
    case 0x13:raw=0x01020100;break;case 0x14:raw=0x01040100;break;
    case 0x15:raw=0x01050100;break;case 0x16:raw=0x01010232;break;
    case 0x17:raw=0x01010332;break;case 0x19:raw=0x01080000;break;
    case 0x1a:raw=0x01090000;break;case 0x1b:raw=0x0107ff32;break;
    case 0x1c:raw=0x0106ff32;break;
    default:return fail(e,n,"Unknown vendor mouse action.");
    }
    if (!a) return fail(e,n,"Missing assignment output.");
    aula_keymap_raw(raw,a);return 0;
}
int aula_keymap_macro(unsigned index,unsigned mode,unsigned repeats,unsigned char a[4],char *e,size_t n) {
    if (!a || index>=AULA_MACRO_MAX_COUNT || (mode!=1 && mode!=2 && mode!=4) || repeats<1 || repeats>255)
        return fail(e,n,"Macro requires index0..99, mode1/2/4 and repeats1..255.");
    a[0]=3;a[1]=(unsigned char)mode;a[2]=(unsigned char)(mode==1?repeats:1);a[3]=(unsigned char)index;return 0;
}
int aula_macro_pack_event(const aula_macro_event *v,unsigned char out[4],char *e,size_t n) {
    if (!v || !out || v->kind>5 || v->delay>AULA_MACRO_MAX_DELAY || (v->pressed!=0 && v->pressed!=1))
        return fail(e,n,"Invalid macro event or delay; maximum delay is1048575ms.");
    if ((v->kind==0 && (v->value<1 || v->value>255)) ||
        (v->kind==1 && (v->value<0xe0 || v->value>0xe7)) ||
        (v->kind==2 && (v->value<1 || v->value>31)) ||
        (v->kind>=3 && (v->value< -128 || v->value>127 || !v->value)))
        return fail(e,n,"Macro event value is outside its supported range.");
    out[0]=(unsigned char)((v->kind<<4)|(v->delay>>16));
    if (v->kind<3 && !v->pressed) out[0]|=0x80;
    out[1]=(unsigned char)(v->delay>>8);out[2]=(unsigned char)v->delay;out[3]=(unsigned char)v->value;return 0;
}
/* Strict UTF8 to the original maximum30 UTF16 code units. */
static int name_utf16(const char *s,unsigned char out[60],size_t *bytes,char *e,size_t n) {
    size_t used=0;const unsigned char *p=(const unsigned char *)s;
    if (!p) return fail(e,n,"Macro name is required.");
    while (*p) {
        unsigned cp=*p++, extra=0,min=0;
        if (cp>=0xc2 && cp<=0xdf) {cp&=31;extra=1;min=0x80;}
        else if (cp>=0xe0 && cp<=0xef) {cp&=15;extra=2;min=0x800;}
        else if (cp>=0xf0 && cp<=0xf4) {cp&=7;extra=3;min=0x10000;}
        else if (cp>=0x80) return fail(e,n,"Macro name is not valid UTF8.");
        for (unsigned i=0;i<extra;i++) {
            if ((*p&0xc0)!=0x80) return fail(e,n,"Macro name is not valid UTF8.");
            cp=(cp<<6)|(*p++&63);
        }
        if (cp<min || cp>0x10ffff || (cp>=0xd800 && cp<=0xdfff)) return fail(e,n,"Macro name is not valid UTF8.");
        size_t need=cp>0xffff?4:2;
        if (used+need>60) return fail(e,n,"Macro name exceeds30 UTF16 code units.");
        if (cp>0xffff) {cp-=0x10000;put16(out+used,0xd800+(cp>>10));put16(out+used+2,0xdc00+(cp&1023));}
        else put16(out+used,cp);
        used+=need;
    }
    *bytes=used;return 0;
}
int aula_macro_pack(const char *name,const aula_macro_event *events,size_t count,unsigned char *out,size_t cap,size_t *len,char *e,size_t n) {
    unsigned char temp[4096],namebytes[60];size_t nb;
    if (!out || !len || !events || !count || count>(4096-61)/4) return fail(e,n,"Macro requires1..1008 events.");
    if (name_utf16(name,namebytes,&nb,e,n)) return -1;
    size_t total=1+nb+count*4;
    if (total>cap || total>4096) return fail(e,n,"Macro does not fit in its output buffer.");
    temp[0]=(unsigned char)nb;memcpy(temp+1,namebytes,nb);
    for (size_t i=0;i<count;i++) if (aula_macro_pack_event(events+i,temp+1+nb+i*4,e,n)) return -1;
    memcpy(out,temp,total);*len=total;return 0;
}
static int valid_blob(const unsigned char *p,size_t len,char *e,size_t n) {
    if (!p || len<5 || p[0]>60 || (p[0]&1) || (size_t)p[0]+1>=len || (len-1-p[0])%4)
        return fail(e,n,"Invalid native macro blob.");
    return 0;
}
int aula_macro_count(const unsigned char pool[4096],unsigned *count,size_t *used,char *e,size_t n) {
    if (!pool || !count || !used) return fail(e,n,"Missing macro pool output.");
    unsigned first=le16(pool);
    if (!first && !le16(pool+2)) {*count=0;*used=0;return 0;}
    if (!first || first%4 || first/4>AULA_MACRO_MAX_COUNT) return fail(e,n,"Unknown macro table layout; existing macros were preserved.");
    unsigned total=first/4;size_t end=first;
    for(unsigned i=0;i<total;i++) {
        unsigned offset=le16(pool+i*4),length=le16(pool+i*4+2);
        if (offset!=end || length>4096-end || valid_blob(pool+offset,length,e,n)) return fail(e,n,"Invalid macro offset table; existing macros were preserved.");
        end+=length;
    }
    *count=total;*used=end;return 0;
}
int aula_macro_append(unsigned char pool[4096],const unsigned char *blob,size_t len,unsigned *index,char *e,size_t n) {
    unsigned total;size_t used;unsigned char temp[4096]={0};
    if (!index || valid_blob(blob,len,e,n) || aula_macro_count(pool,&total,&used,e,n)) return -1;
    if (total>=100 || used>4092 || len>4096-used-4) return fail(e,n,"Keyboard macro memory is full.");
    for(unsigned i=0;i<total;i++) {put16(temp+i*4,le16(pool+i*4)+4);put16(temp+i*4+2,le16(pool+i*4+2));}
    size_t old_data=used-total*4,offset=(total+1)*4+old_data;
    if(total) memcpy(temp+(total+1)*4,pool+total*4,old_data);
    put16(temp+total*4,(unsigned)offset);put16(temp+total*4+2,(unsigned)len);
    memcpy(temp+offset,blob,len);memcpy(pool,temp,4096);*index=total;return 0;
}
int aula_macro_read(aula_hid_device *d,unsigned char pool[4096],char *e,size_t n) {
    unsigned char temp[4096];unsigned total;size_t used;
    if (!pool) return fail(e,n,"Missing macro output.");
    if (aula_g5_read_data(d,0x85,0,temp,4096,e,n) || aula_macro_count(temp,&total,&used,e,n)) return -1;
    memcpy(pool,temp,4096);return 0;
}
int aula_macro_write(aula_hid_device *d,const unsigned char pool[4096],char *e,size_t n) {
    unsigned char actual[4096];unsigned total;size_t used;
    if (aula_macro_count(pool,&total,&used,e,n)) return -1;
    if (aula_g5_write_data(d,5,0,pool,4096,e,n)) return -1;
    wait_written();
    if (aula_macro_read(d,actual,e,n)) return -1;
    if (memcmp(actual,pool,4096)) return fail(e,n,"Macro write verification failed; keyboard may already have changed.");
    return 0;
}
