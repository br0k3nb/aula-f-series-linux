#ifndef AULA_KEYMAP_H
#define AULA_KEYMAP_H
#include "lighting_ext.h"
#include <stdint.h>
#define AULA_KEYMAP_SIZE 512u
#define AULA_KEYMAP_LAYERS 4u
#define AULA_MACRO_SIZE 4096u
#define AULA_MACRO_MAX_COUNT 100u
#define AULA_MACRO_MAX_DELAY 0xfffffu

/* Key records are literal device bytes, in USB transfer order. Physical
 * indexes0..126 may be patched. Index127 contains the trailing5a,a5 marker. */
int aula_keymap_read(aula_hid_device *, unsigned layer,
                     unsigned char map[AULA_KEYMAP_SIZE], char *, size_t);
int aula_keymap_write(aula_hid_device *, unsigned layer,
                      const unsigned char map[AULA_KEYMAP_SIZE], char *, size_t);
int aula_keymap_patch(unsigned char map[AULA_KEYMAP_SIZE], unsigned index,
                      const unsigned char assignment[4], char *, size_t);
/* One or two USB keyboard usages; key2=0 means a single key. modifiers uses
 * the standard eight USB modifier bits. Zero usages allow modifier-only. */
int aula_keymap_key(unsigned modifiers, unsigned key1, unsigned key2,
                    unsigned char assignment[4], char *, size_t);
int aula_keymap_media(unsigned usage, unsigned char assignment[4], char *, size_t);
/* Vendor mouse actions0x11..17,19..1c, matching the original converter. */
int aula_keymap_mouse(unsigned action, unsigned char assignment[4], char *, size_t);
/* Raw code uses canonical visual/vendor order:0x020000cd ->02 00 00 cd.
 * This preserves special original FN commands without reinterpretation. */
void aula_keymap_raw(uint32_t code, unsigned char assignment[4]);
int aula_keymap_macro(unsigned index, unsigned mode, unsigned repeats,
                      unsigned char assignment[4], char *, size_t);

/* Macro events:0 keyboard usage,1 modifier usageE0..E7,2 mouse button mask,
 * 3/4 signed X/Y movement,5 signed wheel movement. pressed0 is release.
 * Delay is milliseconds before the event, encoded as a20-bit integer. */
typedef struct { unsigned kind, delay; int value, pressed; } aula_macro_event;
int aula_macro_pack_event(const aula_macro_event *, unsigned char output[4],
                           char *, size_t);
int aula_macro_pack(const char *utf8_name, const aula_macro_event *, size_t count,
                     unsigned char *output, size_t capacity, size_t *length,
                     char *, size_t);
/* Parses the native contiguous LE16(offset,length) table. Empty pools have
 * zero first entry. Appending preserves every old blob and its index, moves
 * their offsets by4 and leaves inputs unchanged on validation failure. */
int aula_macro_count(const unsigned char pool[AULA_MACRO_SIZE], unsigned *count,
                      size_t *used, char *, size_t);
int aula_macro_append(unsigned char pool[AULA_MACRO_SIZE],
                       const unsigned char *blob, size_t length,
                       unsigned *new_index, char *, size_t);
int aula_macro_read(aula_hid_device *, unsigned char pool[AULA_MACRO_SIZE], char *, size_t);
int aula_macro_write(aula_hid_device *, const unsigned char pool[AULA_MACRO_SIZE], char *, size_t);
#endif
