#define _POSIX_C_SOURCE 200809L
#include "keymap_cli.h"
#include "keymap.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int error_out(const char *error,int status) {fprintf(stderr,"%s\n",error);return status;}
static int number(const char *s,unsigned long max,unsigned *out) {
    char *end;errno=0;
    if(!s||!*s||*s=='-') return -1;
    unsigned long value=strtoul(s,&end,s[0]=='0'&&(s[1]=='x'||s[1]=='X')?16:10);
    if(errno||*end||value>max) return -1;
    *out=(unsigned)value;return 0;
}
static void hex(const unsigned char *p,size_t len) {for(size_t i=0;i<len;i++)printf("%02x",p[i]);}
static unsigned canonical(const unsigned char *p) {return (unsigned)p[0]<<24|(unsigned)p[1]<<16|(unsigned)p[2]<<8|p[3];}
static unsigned u16(const unsigned char *p) {return p[0]|(unsigned)p[1]<<8;}
static void p16(unsigned char *p,unsigned v) {p[0]=(unsigned char)v;p[1]=(unsigned char)(v>>8);}
static int special_allowed(unsigned code) {
    /* Canonical words observed in the shipped FN1 defaults and converter. */
    static const unsigned codes[]={
        0x07000000,0x07000001,0x07000002,0x07000004,0x07000005,0x07000006,0x07000007,
        0x07000008,0x07000009,0x0700000a,0x0700000c,0x07000011,0x07000018,
        0x07000019,0x0700001a,0x0700001b,
        0x08000000,0x08000001,0x08000002,0x08020000,0x08020001,0x08020002,
        0x08030001,0x08030100,0x08030200,0x08040001,0x08040002,0x08040100,0x08040200,
        0x10000002,0x10000004,0x05000007,0x06000007,0x08000007,0x09000007,
        0x0d000000,0x0d010000,0x05010000,0x05020000,0x05040000,0x0e000000};
    for(size_t i=0;i<sizeof(codes)/sizeof(codes[0]);i++)if(code==codes[i])return 1;
    return 0;
}
static unsigned mouse_action(const unsigned char *p) {
    unsigned char a[4];char e[2];
    for(unsigned c=0x11;c<=0x1c;c++)if(!aula_keymap_mouse(c,a,e,sizeof(e))&&!memcmp(p,a,4))return c;
    return canonical(p);
}
static void show_map(const unsigned char map[512],unsigned layer,int verified) {
    printf("{\"layer\":%u,\"verified\":%s,\"keys\":[",layer,verified?"true":"false");
    for(unsigned i=0;i<127;i++) {
        const unsigned char *a=map+i*4;const char *type="special";unsigned code=canonical(a),code2=0,mod=0;
        if(!code)type="disable";
        else if(a[0]==0) {type=a[2]||a[1]?"combo":"key";code=a[3];code2=a[2];mod=a[1];}
        else if(a[0]==2&&a[1]==0) {type="media";code=(unsigned)a[2]<<8|a[3];}
        else if(a[0]==1) {type="mouse";code=mouse_action(a);}
        else if(a[0]==3) {type="macro";code=a[3];}
        printf("%s{\"index\":%u,\"type\":\"%s\",\"code\":%u,\"code2\":%u,\"modifiers\":%u,\"raw\":\"",i?",":"",i,type,code,code2,mod);
        hex(a,4);printf("\"");
        if(a[0]==3)printf(",\"repeat\":%u,\"trigger\":\"%s\"",a[2],a[1]==2?"hold":a[1]==4?"toggle":"once");
        putchar('}');
    }
    printf("],\"raw_hex\":\"");hex(map,512);puts("\"}");
}
static void show_macros(const unsigned char p[4096],unsigned count,size_t used) {
    printf("{\"capacity\":4096,\"used\":%zu,\"macros\":[",used);
    for(unsigned i=0;i<count;i++) {
        unsigned offset=u16(p+i*4),len=u16(p+i*4+2);const unsigned char *b=p+offset;
        printf("%s{\"index\":%u,\"name\":\"",i?",":"",i);
        for(unsigned j=0;j<b[0];j+=2)printf("\\u%04x",u16(b+1+j));
        printf("\",\"events\":[");unsigned ev=0;
        for(unsigned j=1+b[0];j+3<len;j+=4) {
            unsigned kind=(b[j]>>4)&7,delay=(unsigned)(b[j]&15)<<16|(unsigned)b[j+1]<<8|b[j+2];
            int value=kind>=3?(int)(signed char)b[j+3]:b[j+3];
            printf("%s{\"kind\":%u,\"code\":%d,\"pressed\":%s,\"delay_ms\":%u}",ev++?",":"",kind,value,b[j]&128?"false":"true",delay);
        }
        printf("]}");
    }
    puts("]}");
}
static int read_events(const char *path,aula_macro_event *events,size_t *count,char *e,size_t n) {
    FILE *f=fopen(path,"r");char *line=NULL;size_t cap=0,total=0,line_no=0;int result=-1;
    if(!f) {snprintf(e,n,"Cannot open macro events file: %s",strerror(errno));return -1;}
    while(getline(&line,&cap,f)>=0) {
        line_no++;char *p=line;while(*p==' '||*p=='\t')p++;
        if(*p=='#'||*p=='\n'||!*p)continue;
        if(total>=1008) {snprintf(e,n,"Macro events exceed1008 actions.");goto done;}
        long v[4];
        for(unsigned i=0;i<4;i++) {
            char *end;errno=0;v[i]=strtol(p,&end,0);
            if(errno||end==p) {snprintf(e,n,"Invalid macro CSV at line%zu.",line_no);goto done;}
            p=end;while(*p==' '||*p=='\t')p++;
            if(i<3) {if(*p++!=',') {snprintf(e,n,"Expected four CSV fields at line%zu.",line_no);goto done;}}
        }
        while(*p==' '||*p=='\t'||*p=='\r'||*p=='\n')p++;
        if(*p||v[0]<0||v[0]>5||v[1]< -128||v[1]>255||v[2]<0||v[2]>1||v[3]<0||v[3]>AULA_MACRO_MAX_DELAY) {
            snprintf(e,n,"Invalid macro CSV values at line%zu.",line_no);goto done;
        }
        events[total++]=(aula_macro_event){(unsigned)v[0],(unsigned)v[3],(int)v[1],(int)v[2]};
    }
    if(ferror(f)) {snprintf(e,n,"Cannot read macro events file.");goto done;}
    if(!total) {snprintf(e,n,"Macro events file contains no actions.");goto done;}
    *count=total;result=0;
 done:free(line);fclose(f);return result;
}
int aula_keymap_cli(int argc,char **argv) {
    if(argc<2)return -1;
    int read=!strcmp(argv[1],"--keymap"),remap=!strcmp(argv[1],"--remap"),list=!strcmp(argv[1],"--macro-list"),add=!strcmp(argv[1],"--macro-add"),del=!strcmp(argv[1],"--macro-delete"),update=!strcmp(argv[1],"--macro-update");
    if(!read&&!remap&&!list&&!add&&!del&&!update)return -1;
    if(argc<3)return error_out("A HID device node is required.",2);
    const char *node=argv[2],*action=NULL,*name=NULL,*events_file=NULL,*trigger="once";
    unsigned layer=0,key=UINT_MAX,code=UINT_MAX,code2=0,modifiers=0,repeats=1,delete_index=UINT_MAX;
    unsigned seen=0;char error[512]={0};
    for(int i=3;i<argc;i+=2) {
        if(i+1==argc)return error_out("Every option requires a value.",2);
        const char *arg=argv[i],*value=argv[i+1];unsigned bit;
        if(!strcmp(arg,"--layer")) {bit=1;if(number(value,3,&layer))goto invalid;}
        else if(!strcmp(arg,"--key")) {bit=2;if(number(value,126,&key))goto invalid;}
        else if(!strcmp(arg,"--code")) {bit=4;if(number(value,UINT_MAX,&code))goto invalid;}
        else if(!strcmp(arg,"--code2")) {bit=8;if(number(value,255,&code2))goto invalid;}
        else if(!strcmp(arg,"--modifiers")) {bit=16;if(number(value,255,&modifiers))goto invalid;}
        else if(!strcmp(arg,"--repeat")) {bit=32;if(number(value,255,&repeats)||!repeats)goto invalid;}
        else if(!strcmp(arg,"--trigger")) {bit=64;trigger=value;if(strcmp(value,"once")&&strcmp(value,"hold")&&strcmp(value,"toggle"))goto invalid;}
        else if(!strcmp(arg,"--action")) {bit=128;action=value;}
        else if(!strcmp(arg,"--name")) {bit=256;name=value;}
        else if(!strcmp(arg,"--events")) {bit=512;events_file=value;}
        else if(!strcmp(arg,"--index")) {bit=1024;if(number(value,99,&delete_index))goto invalid;}
        else goto invalid;
        if(seen&bit)goto invalid;
        seen|=bit;
    }
    if((read&&(seen&~1u))||(list&&seen)||(del&&(seen!=1024))||(update&&(seen!=(1024u|256u|512u)))||
       (remap&&(!action||key==UINT_MAX||(seen&~255u)))||
       (add&&(!name||!events_file||key==UINT_MAX||(seen&~(1u|2u|32u|64u|256u|512u)))))goto invalid;
    unsigned char assignment[4]={0},map[512],pool[4096],blob[4096];unsigned macro_index=0,total=0;size_t used=0,blob_len=0;
    unsigned mode=!strcmp(trigger,"hold")?2:!strcmp(trigger,"toggle")?4:1;
    if(remap) {
        if(!strcmp(action,"disable")) {if(seen&(4|8|16|32|64))goto invalid;}
        else if(!strcmp(action,"key")||!strcmp(action,"combo")) {
            if(code==UINT_MAX||(seen&(32|64))||aula_keymap_key(modifiers,code,code2,assignment,error,sizeof(error)))goto invalid;
        }
        else if(!strcmp(action,"media")) {if(code==UINT_MAX||(seen&(8|16|32|64))||aula_keymap_media(code,assignment,error,sizeof(error)))goto invalid;}
        else if(!strcmp(action,"mouse")) {if(code==UINT_MAX||(seen&(8|16|32|64))||aula_keymap_mouse(code,assignment,error,sizeof(error)))goto invalid;}
        else if(!strcmp(action,"special")) {if(code==UINT_MAX||!special_allowed(code)||(seen&(8|16|32|64)))goto invalid;aula_keymap_raw(code,assignment);}
        else if(!strcmp(action,"macro")) {if(code==UINT_MAX||(seen&(8|16))||aula_keymap_macro(code,mode,repeats,assignment,error,sizeof(error)))goto invalid;}
        else goto invalid;
    }
    if(add||update) {
        aula_macro_event *events=calloc(1008,sizeof(*events));size_t count=0;
        if(!events)return error_out("Cannot allocate macro events.",1);
        int r=read_events(events_file,events,&count,error,sizeof(error));
        if(!r)r=aula_macro_pack(name,events,count,blob,sizeof(blob),&blob_len,error,sizeof(error));
        free(events);if(r)return error_out(error,2);
    }
    aula_hid_device device;
    if(aula_hid_open(node,1,&device,error,sizeof(error)))return error_out(error,1);
    int r=0;
    if(read||remap||add)r=aula_keymap_read(&device,layer,map,error,sizeof(error));
    if(!r&&(list||add||del||update||(remap&&!strcmp(action,"macro")))) {
        r=aula_macro_read(&device,pool,error,sizeof(error));
        if(!r)r=aula_macro_count(pool,&total,&used,error,sizeof(error));
    }
    if(!r&&remap&&!strcmp(action,"macro")&&code>=total) {snprintf(error,sizeof(error),"Macro index does not exist.");r=-1;}
    if(!r&&add) {
        r=aula_macro_append(pool,blob,blob_len,&macro_index,error,sizeof(error));
        if(!r)r=aula_keymap_macro(macro_index,mode,repeats,assignment,error,sizeof(error));
        if(!r)r=aula_macro_write(&device,pool,error,sizeof(error));
    }
    if(!r&&(remap||add)) {
        r=aula_keymap_patch(map,key,assignment,error,sizeof(error));
        if(!r)r=aula_keymap_write(&device,layer,map,error,sizeof(error));
        if(r&&add) {size_t l=strlen(error);snprintf(error+l,sizeof(error)-l," Macro was stored as index%u; key assignment may be incomplete.",macro_index);}
    }
    if(!r&&update) {
        if(delete_index>=total) {snprintf(error,sizeof(error),"Macro index does not exist.");r=-1;}
        if(!r) {
            unsigned old_length=u16(pool+delete_index*4+2);
            if(blob_len>4096-(used-old_length)) {snprintf(error,sizeof(error),"Updated macro does not fit in keyboard memory.");r=-1;}
        }
        if(!r) {
            unsigned char next[4096]={0};size_t cursor=total*4;
            for(unsigned j=0;j<total;j++) {
                unsigned offset=u16(pool+j*4),length=u16(pool+j*4+2);
                const unsigned char *data=pool+offset;
                if(j==delete_index) {data=blob;length=(unsigned)blob_len;}
                p16(next+j*4,(unsigned)cursor);p16(next+j*4+2,length);
                memcpy(next+cursor,data,length);cursor+=length;
            }
            r=aula_macro_write(&device,next,error,sizeof(error));
        }
    }
    if(!r&&del) {
        /* Removing an unassigned macro preserves every other macro index.
         * Removing an assigned one requires the user to remap that key first. */
        if(delete_index>=total) {snprintf(error,sizeof(error),"Macro index does not exist.");r=-1;}
        unsigned char layers[4][512];
        for(unsigned l=0;!r&&l<4;l++) {
            r=aula_keymap_read(&device,l,layers[l],error,sizeof(error));
            for(unsigned k=0;!r&&k<127;k++)if(layers[l][k*4]==3&&layers[l][k*4+3]>=delete_index) {
                snprintf(error,sizeof(error),"Macro%u or a later macro is assigned to layer%u,key%u. Unassign those macros before deletion.",delete_index,l,k);r=-1;
            }
        }
        if(!r) {
            unsigned char next[4096]={0};unsigned out_index=0;size_t cursor=(total-1)*4;
            for(unsigned j=0;j<total;j++)if(j!=delete_index) {
                unsigned offset=u16(pool+j*4),length=u16(pool+j*4+2);
                p16(next+out_index*4,(unsigned)cursor);p16(next+out_index*4+2,length);
                memcpy(next+cursor,pool+offset,length);cursor+=length;out_index++;
            }
            r=aula_macro_write(&device,next,error,sizeof(error));
        }
    }
    aula_hid_close(&device);
    if(r)return error_out(error,1);
    if(read||remap)show_map(map,layer,remap);
    else if(list)show_macros(pool,total,used);
    else if(update)printf("{\"index\":%u,\"updated\":true,\"verified\":true}\n",delete_index);
    else if(add)printf("{\"index\":%u,\"layer\":%u,\"key\":%u,\"verified\":true}\n",macro_index,layer,key);
    else printf("{\"deleted\":%u,\"verified\":true}\n",delete_index);
    return 0;
 invalid:return error_out(*error?error:"Invalid keymap or macro arguments.",2);
}
