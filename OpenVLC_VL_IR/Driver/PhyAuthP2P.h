#ifndef PHYAUTHP2P_H
#define PHYAUTHP2P_H

#include <stdint.h>   // for uint8_t, uint32_t
#include <stddef.h>   // for size_t

#define POTP_LEN 4 // 4 bytes output

static int hmac_sha256(const u8 *key, unsigned int keylen,
                       const u8 *data, unsigned int datalen,
                       u8 *out, unsigned int outlen);

static u32 truncate(const u8 *hmac_result, size_t hmac_len);

int generate_potp(u8 *out);

static void example_potp_usage(void);


#endif