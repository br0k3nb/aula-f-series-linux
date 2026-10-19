#ifndef AULA_PROTOCOL_H
#define AULA_PROTOCOL_H
#include "hid_linux.h"
#define AULA_CFG_SIZE 128u
#define AULA_PACKET_SIZE 520u
#define AULA_FEATURE_ID 6u

enum aula_model { AULA_MODEL_F75, AULA_MODEL_F87, AULA_MODEL_F99 };
/* Pure packet formatter for the observed wired G5 single-chunk protocol.
 * cmd is 0x84 (read) or 0x04 (write); payload is exactly 128 bytes, or NULL
 * for a zero-filled read request. No checksum is added. */
int aula_protocol_build_packet(unsigned cmd, const unsigned char *payload,
                               unsigned char packet[AULA_PACKET_SIZE],
                               char *error, size_t error_len);
/* All hardware operations require wired 258a:010c and a descriptor declaring
 * feature ID6 with exactly 520 ioctl bytes. Wireless is unsupported.
 * Reads demand the echoed request header and 5a,a5 config tail. The observed
 * wired keyboard returns 136 bytes; padded responses up to 520 are accepted. */
int aula_protocol_read_cfg(aula_hid_device *device,
                           unsigned char cfg[AULA_CFG_SIZE],
                           char *error, size_t error_len);
/* Pure read-modify-write patch for shipped SubFw=0 defaults. Each argument
 * -1 preserves its field; mode supports 0..18. Explicit changes select normal
 * lighting (cfg[9]=0, cfg[38h]=cfg[39h]=ff). Mode0 bypasses effect table,
 * so brightness/speed have no effect while off. Other bytes stay intact;
 * speed preserves the random-color low nibble. No color bytes are changed.
 * Invalid input leaves cfg unchanged. Existing custom mode21 requires an
 * explicit supported mode when brightness or speed is being changed. */
int aula_protocol_patch_cfg(unsigned char cfg[AULA_CFG_SIZE], enum aula_model model,
                            int mode, int brightness, int speed,
                            char *error, size_t error_len);
/* Writes the entire supplied config, waits, reads back, and verifies exact
 * equality. Failure may mean the device already accepted the write: no
 * automatic retry, rollback, or factory reset is performed. */
int aula_protocol_write_cfg(aula_hid_device *device,
                            const unsigned char cfg[AULA_CFG_SIZE],
                            char *error, size_t error_len);
#endif
