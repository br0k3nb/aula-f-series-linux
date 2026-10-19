#define _POSIX_C_SOURCE 200809L
#include "hid_linux.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/hidraw.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

static int fail(char *e, size_t n, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    if (e && n) vsnprintf(e, n, fmt, ap);
    va_end(ap);
    return -1;
}
int aula_hid_supported(uint16_t v, uint16_t p) {
    return (v == 0x258a && p == 0x010c) || (v == 0x3554 && p == 0xfa09);
}

typedef struct { uint32_t page, size, count, id; } globals;
int aula_hid_parse_descriptor(const unsigned char *data, size_t len,
                              aula_hid_reports *out, char *e, size_t en) {
    globals g = {0,0,0,0}, stack[32];
    size_t depth = 0, pos = 0;
    if (!out || (!data && len) || !len || len > HID_MAX_DESCRIPTOR_SIZE)
        return fail(e,en,"Invalid descriptor length");
    memset(out,0,sizeof(*out));
    while (pos < len) {
        unsigned prefix = data[pos++], count, type, tag;
        uint32_t val = 0;
        if (prefix == 0xfe) {
            size_t skip;
            if (len-pos < 2) return fail(e,en,"Truncated long HID item");
            skip = data[pos]; pos += 2;
            if (skip > len-pos) return fail(e,en,"Truncated long HID payload");
            pos += skip; continue;
        }
        count = prefix & 3u; if (count == 3) count = 4;
        type = (prefix >> 2) & 3u; tag = prefix >> 4;
        if (count > len-pos) return fail(e,en,"Truncated HID item");
        for (unsigned i=0;i<count;i++) val |= (uint32_t)data[pos+i] << (8*i);
        pos += count;
        if (type == 1) {
            switch(tag) {
            case 0: g.page=val; break;
            case 7: g.size=val; break;
            case 8:
                if (!val || val > 255) return fail(e,en,"Invalid HID report ID");
                g.id=val; out->numbered=1; break;
            case 9: g.count=val; break;
            case 10:
                if (depth == 32) return fail(e,en,"HID global stack overflow");
                stack[depth++]=g; break;
            case 11:
                if (!depth) return fail(e,en,"HID global stack underflow");
                g=stack[--depth]; break;
            default: break;
            }
        } else if (type == 0 && tag == 11) {
            aula_hid_report *r=&out->feature[g.id];
            size_t bits;
            /* Kernel hidraw ioctl size is encoded in 14 bits on Linux.
             * Cap below that and avoid uint32 multiplication overflow. */
            if (!g.size || !g.count || g.size > 131056u || g.count > 131056u ||
                g.size > 131056u/g.count)
                return fail(e,en,"Invalid or oversized feature report");
            bits=(size_t)g.size*g.count;
            if (r->payload_bits > 131056u-bits)
                return fail(e,en,"Oversized aggregate feature report");
            if (!r->present) r->usage_page=g.page;
            else if (r->usage_page != g.page) r->mixed_usage_pages=1;
            r->present=1; r->payload_bits+=bits;
        }
    }
    if (depth) return fail(e,en,"Unbalanced HID global stack");
    if (out->numbered && out->feature[0].present)
        return fail(e,en,"Mixed numbered and unnumbered feature reports");
    for (unsigned i=0;i<256;i++) {
        aula_hid_report *r=&out->feature[i];
        if (r->present) {
            r->payload_bytes=(r->payload_bits+7)/8;
            r->ioctl_bytes=r->payload_bytes+1;
            r->wire_bytes=r->payload_bytes+(out->numbered ? 1u : 0u);
        }
    }
    return 0;
}

static int read_file(const char *p, unsigned char *b, size_t cap, size_t *n) {
    int fd=open(p,O_RDONLY|O_CLOEXEC); size_t used=0;
    if(fd<0) return -1;
    while(used<cap) {
        ssize_t got=read(fd,b+used,cap-used);
        if(got<0) { if(errno==EINTR) continue; close(fd); return -1; }
        if(!got) { close(fd); *n=used; return 0; }
        used+=(size_t)got;
    }
    { unsigned char extra; ssize_t got;
      do { got=read(fd,&extra,1); } while(got<0 && errno==EINTR);
      close(fd); if(got!=0) { errno=EFBIG; return -1; }
    }
    *n=used; return 0;
}
static int valid_name(const char *s) {
    if(strncmp(s,"hidraw",6) || !s[6]) return 0;
    for(s+=6;*s;s++) if(*s<'0'||*s>'9') return 0;
    return 1;
}
int aula_hid_discover(aula_hid_found_fn found, void *user, char *e, size_t en) {
    DIR *dir=opendir("/sys/class/hidraw"); struct dirent *de; int count=0;
    if(!dir) { if(errno==ENOENT) return 0; return fail(e,en,"Cannot enumerate hidraw: %s",strerror(errno)); }
    while((de=readdir(dir))) {
        char path[512], text[4097], *line, *save=NULL;
        unsigned char desc[HID_MAX_DESCRIPTOR_SIZE]; size_t n;
        unsigned bus=0,v=0,p=0; int have_id=0; struct stat st;
        aula_hid_info info; memset(&info,0,sizeof(info));
        if(!valid_name(de->d_name) || strlen(de->d_name)>100) continue;
        snprintf(path,sizeof(path),"/sys/class/hidraw/%s/device/uevent",de->d_name);
        if(read_file(path,(unsigned char *)text,sizeof(text)-1,&n)<0) continue;
        text[n]=0;
        for(line=strtok_r(text,"\n",&save);line;line=strtok_r(NULL,"\n",&save)) {
            if(sscanf(line,"HID_ID=%x:%x:%x",&bus,&v,&p)==3) have_id=1;
            if(!strncmp(line,"HID_NAME=",9)) snprintf(info.name,sizeof(info.name),"%s",line+9);
        }
        if(!have_id || v>65535 || p>65535 || !aula_hid_supported((uint16_t)v,(uint16_t)p)) continue;
        info.vendor=(uint16_t)v; info.product=(uint16_t)p;
        snprintf(info.node,sizeof(info.node),"/dev/%.100s",de->d_name);
        info.node_exists=lstat(info.node,&st)==0 && S_ISCHR(st.st_mode);
        info.readable=info.node_exists && access(info.node,R_OK)==0;
        info.writable=info.node_exists && access(info.node,W_OK)==0;
        snprintf(path,sizeof(path),"/sys/class/hidraw/%s/device/report_descriptor",de->d_name);
        if(read_file(path,desc,sizeof(desc),&n)==0)
            info.descriptor_ok=aula_hid_parse_descriptor(desc,n,&info.reports,NULL,0)==0;
        count++; if(found) found(&info,user);
    }
    closedir(dir); return count;
}
int aula_hid_open(const char *node, int writable, aula_hid_device *out, char *e, size_t en) {
    struct stat st; struct hidraw_devinfo raw; struct hidraw_report_descriptor rd;
    int fd,sz=0;
    if(!out) return fail(e,en,"Missing output handle");
    memset(out,0,sizeof(*out)); out->fd=-1;
    if(!node || strncmp(node,"/dev/",5) || !valid_name(node+5))
        return fail(e,en,"Expected explicit /dev/hidrawN node");
    fd=open(node,(writable ? O_RDWR : O_RDONLY)|O_CLOEXEC|O_NOFOLLOW);
    if(fd<0) return fail(e,en,"Cannot open %s: %s",node,strerror(errno));
    memset(&raw,0,sizeof(raw)); memset(&rd,0,sizeof(rd));
    if(fstat(fd,&st)<0 || !S_ISCHR(st.st_mode)) {
        close(fd); return fail(e,en,"Node is not a character device");
    }
    if(writable && flock(fd,LOCK_EX|LOCK_NB)<0) {
        close(fd); return fail(e,en,"Keyboard is busy in another operation. Stop live lighting and try again.");
    }
    if(ioctl(fd,HIDIOCGRAWINFO,&raw)<0 ||
       !aula_hid_supported((uint16_t)raw.vendor,(uint16_t)raw.product)) {
        close(fd); return fail(e,en,"Device identity is unsupported or unavailable");
    }
    if(ioctl(fd,HIDIOCGRDESCSIZE,&sz)<0 || sz<=0 || sz>HID_MAX_DESCRIPTOR_SIZE) {
        close(fd); return fail(e,en,"Cannot obtain valid HID descriptor size");
    }
    rd.size=(unsigned)sz;
    if(ioctl(fd,HIDIOCGRDESC,&rd)<0 || rd.size>(unsigned)sz ||
       aula_hid_parse_descriptor(rd.value,rd.size,&out->reports,e,en)<0) {
        close(fd); return fail(e,en,"Cannot obtain valid HID descriptor");
    }
    out->fd=fd; out->writable=!!writable;
    out->vendor=(uint16_t)raw.vendor; out->product=(uint16_t)raw.product;
    return 0;
}
void aula_hid_close(aula_hid_device *d) { if(d && d->fd>=0) { close(d->fd); d->fd=-1; } }
static int feature(aula_hid_device *d, unsigned id, unsigned char *b, size_t n,
                   int set, char *e, size_t en) {
    int rc;
    if(!d || d->fd<0 || id>255 || !b || !d->reports.feature[id].present ||
       n!=d->reports.feature[id].ioctl_bytes || n>16383 || (set && !d->writable))
        return fail(e,en,"Invalid feature report, size, or access mode");
    b[0]=(unsigned char)id;
    do { rc=ioctl(d->fd,set ? HIDIOCSFEATURE(n) : HIDIOCGFEATURE(n),b); }
    while(rc<0 && errno==EINTR);
    if(rc<0) return fail(e,en,"Feature %s failed: %s",set?"write":"read",strerror(errno));
    return rc;
}
int aula_hid_get_feature(aula_hid_device *d,unsigned id,unsigned char *b,size_t n,char *e,size_t en) {
    return feature(d,id,b,n,0,e,en);
}
int aula_hid_set_feature(aula_hid_device *d,unsigned id,unsigned char *b,size_t n,char *e,size_t en) {
    return feature(d,id,b,n,1,e,en);
}
