#ifndef AULA_LIGHTING_EXT_H
#define AULA_LIGHTING_EXT_H
#include "protocol.h"

#define AULA_RGB_SIZE 512u
#define AULA_RGB_PALETTE_COLORS 7u
#define AULA_PERKEY_COUNT 126u
#define AULA_PERKEY_SIZE (AULA_PERKEY_COUNT * 3u)
#define AULA_G5_CHUNK_SIZE 512u
#define AULA_G5_MAX_DATA (255u * AULA_G5_CHUNK_SIZE)

/* Generic observed wired G5 AccessData transport. Each report has an 8-byte
 * header and at most 512 bytes of payload. Reads validate every echoed header
 * and transfer length and leave output unchanged on failure. Writes do not
 * verify or retry; callers must use the matching read command and compare.
 * Failed writes may already have changed the device. */
int aula_g5_build_packet(unsigned cmd, unsigned sub, unsigned total,
                         unsigned index, const unsigned char *payload,
                         size_t length, unsigned char packet[AULA_PACKET_SIZE],
                         char *error, size_t error_len);
int aula_g5_read_data(aula_hid_device *device, unsigned cmd, unsigned sub,
                      unsigned char *output, size_t length,
                      char *error, size_t error_len);
int aula_g5_write_data(aula_hid_device *device, unsigned cmd, unsigned sub,
                       const unsigned char *payload, size_t length,
                       char *error, size_t error_len);

int aula_lighting_read_rgb(aula_hid_device *device,
                           unsigned char rgb[AULA_RGB_SIZE],
                           char *error, size_t error_len);
/* Writes command 0x0a and verifies exact equality through command 0x8a. */
int aula_lighting_write_rgb(aula_hid_device *device,
                            const unsigned char rgb[AULA_RGB_SIZE],
                            char *error, size_t error_len);
/* Stored custom lighting uses three planes: all red, all green, all blue.
 * It has no marker or checksum. Command 6 writes and 0x86 verifies group0. */
int aula_lighting_read_perkey(aula_hid_device *device,
                              unsigned char colors[AULA_PERKEY_SIZE],
                              char *error, size_t error_len);
int aula_lighting_write_perkey(aula_hid_device *device,
                               const unsigned char colors[AULA_PERKEY_SIZE],
                               char *error, size_t error_len);
int aula_lighting_patch_perkey(unsigned char colors[AULA_PERKEY_SIZE],
                               unsigned matrix_index, unsigned red,
                               unsigned green, unsigned blue,
                               char *error, size_t error_len);
/* Selects the shipped custom effect without altering ordinary mode tables. */
int aula_lighting_select_custom(unsigned char cfg[AULA_CFG_SIZE],
                                char *error, size_t error_len);
/* Live frames use INTERLEAVED RGB, unlike stored per-key colors. This sends
 * command8, sub0 without a persistent config write or a read-back command.
 * Stop streaming to relinquish host updates; restore saved cfg separately. */
int aula_lighting_send_stream(aula_hid_device *device,
                              const unsigned char frame[AULA_PERKEY_SIZE],
                              char *error, size_t error_len);
/* Verified against a user-operated Fn+LeftShift toggle: cfg[0x12] changes
 * from 1 (on) to 0 (off). -1 preserves it. No other bytes are changed. */
int aula_lighting_patch_indicator(unsigned char cfg[AULA_CFG_SIZE], int on,
                                  char *error, size_t error_len);
/* Pure patches. Only the selected mode/color or config field is modified.
 * Invalid input leaves all supplied bytes unchanged. Hardware modes 1..18
 * use records at mode*21, with seven RGB triplets in each record. */
int aula_lighting_patch_palette(unsigned char rgb[AULA_RGB_SIZE], int mode,
                                int color_index, unsigned red, unsigned green,
                                unsigned blue, char *error, size_t error_len);
int aula_lighting_patch_colors(unsigned char rgb[AULA_RGB_SIZE], int mode,
                               const unsigned char colors[21],
                               char *error, size_t error_len);
/* color_index 0..6 chooses a palette entry; 7 selects multicolor. Speed's
 * high nibble is preserved. This does not change the current hardware mode. */
int aula_lighting_patch_color_selection(unsigned char cfg[AULA_CFG_SIZE],
                                         int mode, int color_index,
                                         char *error, size_t error_len);
/* -1 preserves a field. Debounce is 1..5 ms, encoded minus one. Sleep is
 * off(0), or60,90,120,180,240,300,600,900,1200 seconds for F75/F99. F87 has no
 * shipped explicit sleep table and an explicit sleep change is rejected. */
int aula_lighting_patch_settings(unsigned char cfg[AULA_CFG_SIZE],
                                  enum aula_model model, int debounce_ms,
                                  int sleep_seconds,
                                  char *error, size_t error_len);
#endif
