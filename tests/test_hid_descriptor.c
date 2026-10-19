#include "../src/hid_linux.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    aula_hid_reports r;
    char error[128];
    /* Vendor page FF00; report ID 5; two 8-bit feature fields plus a
     * 3-bit feature field accumulated across PUSH/POP state changes. */
    const unsigned char numbered[]={0x06,0x00,0xff,0x85,5,0x75,8,0x95,2,
        0xb1,2,0xa4,0x75,1,0x95,3,0xb1,2,0xb4};
    const unsigned char unnumbered[]={0x05,1,0x75,8,0x95,64,0xb1,2};
    const unsigned char mixed[]={0x75,8,0x95,1,0xb1,2,0x85,1,0xb1,2};
    const unsigned char overflow[]={0x77,0xff,0xff,0xff,0xff,0x95,2,0xb1,2};
    const unsigned char badpop[]={0xb4};
    const unsigned char longbad[]={0xfe,4,1,0};
    const unsigned char pages[]={0x85,2,0x75,8,0x95,1,0x05,1,0xb1,2,0x05,2,0xb1,2};
    assert(aula_hid_parse_descriptor(numbered,sizeof(numbered),&r,error,sizeof(error))==0);
    assert(r.numbered && r.feature[5].payload_bits==19);
    assert(r.feature[5].payload_bytes==3 && r.feature[5].ioctl_bytes==4);
    assert(r.feature[5].wire_bytes==4 && r.feature[5].usage_page==0xff00);
    assert(aula_hid_parse_descriptor(unnumbered,sizeof(unnumbered),&r,error,sizeof(error))==0);
    assert(!r.numbered && r.feature[0].wire_bytes==64 && r.feature[0].ioctl_bytes==65);
    assert(aula_hid_parse_descriptor(mixed,sizeof(mixed),&r,error,sizeof(error))<0);
    assert(aula_hid_parse_descriptor(overflow,sizeof(overflow),&r,error,sizeof(error))<0);
    assert(aula_hid_parse_descriptor(badpop,sizeof(badpop),&r,error,sizeof(error))<0);
    assert(aula_hid_parse_descriptor(longbad,sizeof(longbad),&r,error,sizeof(error))<0);
    assert(aula_hid_parse_descriptor(numbered,1,&r,error,sizeof(error))<0);
    assert(aula_hid_parse_descriptor(pages,sizeof(pages),&r,error,sizeof(error))==0);
    assert(r.feature[2].mixed_usage_pages && r.feature[2].payload_bytes==2);
    assert(aula_hid_supported(0x258a,0x010c));
    assert(aula_hid_supported(0x3554,0xfa09));
    assert(!aula_hid_supported(0x258a,0x0001));
    puts("HID descriptor fixtures passed");
    return 0;
}
