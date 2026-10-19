#ifndef AULA_HID_LINUX_H
#define AULA_HID_LINUX_H
#include <stddef.h>
#include <stdint.h>

/* Feature reports only. This module never reads keyboard input reports.
 * ioctl_bytes includes the leading report-ID byte required by hidraw even
 * for unnumbered report 0; wire_bytes includes it only for numbered reports.
 * A report can contain fields from multiple pages; mixed_usage_pages marks
 * this case and usage_page retains the first feature field's page. */
typedef struct {
    int present;
    uint32_t usage_page;
    int mixed_usage_pages;
    size_t payload_bits, payload_bytes, wire_bytes, ioctl_bytes;
} aula_hid_report;
typedef struct {
    int numbered;
    aula_hid_report feature[256];
} aula_hid_reports;
typedef struct {
    char node[256], name[256];
    uint16_t vendor, product;
    int node_exists, readable, writable, descriptor_ok;
    aula_hid_reports reports;
} aula_hid_info;
typedef struct {
    int fd, writable;
    uint16_t vendor, product;
    aula_hid_reports reports;
} aula_hid_device;

int aula_hid_supported(uint16_t vendor, uint16_t product);
/* Strict parser rejects truncated items, unbalanced global push/pop,
 * overflow, mixed unnumbered/numbered feature reports and unreasonable sizes.
 * Returns 0 on success, -1 on failure, writing a human-readable error. */
int aula_hid_parse_descriptor(const unsigned char *data, size_t len,
                              aula_hid_reports *out, char *error, size_t error_len);
/* Enumerates matching /sys/class/hidraw entries without opening /dev nodes.
 * Missing nodes are still reported; descriptor_ok indicates readable valid
 * sysfs descriptor. Callback data is valid only during the callback.
 * Returns count, or -1 for enumeration failure. No devices is success (0). */
typedef void (*aula_hid_found_fn)(const aula_hid_info *, void *);
int aula_hid_discover(aula_hid_found_fn found, void *user, char *error, size_t error_len);
/* Explicit /dev/hidrawN only; rejects symlinks and non-character devices,
 * verifies matching VID/PID via HIDIOCGRAWINFO, parses descriptor via ioctl.
 * A successful handle must be closed with aula_hid_close. */
int aula_hid_open(const char *node, int writable, aula_hid_device *out,
                  char *error, size_t error_len);
void aula_hid_close(aula_hid_device *device);
/* Buffer length MUST equal descriptor feature[id].ioctl_bytes. These
 * functions set buffer[0] to id. GET returns received byte count, SET returns
 * ioctl byte count, or -1 on error. SET requires a writable handle. The caller
 * alone supplies protocol semantics and decides whether a report is safe. */
int aula_hid_get_feature(aula_hid_device *, unsigned id, unsigned char *buffer,
                         size_t len, char *error, size_t error_len);
int aula_hid_set_feature(aula_hid_device *, unsigned id, unsigned char *buffer,
                         size_t len, char *error, size_t error_len);
#endif
