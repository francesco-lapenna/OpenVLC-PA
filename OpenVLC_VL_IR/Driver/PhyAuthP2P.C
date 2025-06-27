#include "PhyAuthP2P.h"

/*******************************************************************************/
/****   POTP Generation   ******************************************************/
/*******************************************************************************/

#include <crypto/hash.h>
#include <linux/crypto.h>
#include <linux/scatterlist.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/random.h>

// informations supposed to be known by both sender and receiver:
static const char* PSK = "0123456789abcdef"; // Example PSK
static const unsigned short src_addr = 0x1234; // Example source address
static const int X = 3;  // Time step in seconds
static const int T0 = 1747636421;  // Example start time in Unix
static int T_prev = 0; // Previous time step, initialized to 0
static int SN = 0; // Sequence Number, initialized to 0


static int hmac_sha256(const u8 *key, unsigned int keylen,
                       const u8 *data, unsigned int datalen,
                       u8 *out, unsigned int outlen)
{
    struct crypto_shash *tfm;
    struct shash_desc *shash;
    int ret = 0;

    tfm = crypto_alloc_shash("hmac(sha256)", 0, 0);
    if (IS_ERR(tfm))
        return PTR_ERR(tfm);

    shash = kmalloc(sizeof(*shash) + crypto_shash_descsize(tfm), GFP_KERNEL);
    if (!shash) {
        crypto_free_shash(tfm);
        return -ENOMEM;
    }

    shash->tfm = tfm;
    shash->flags = 0;

    ret = crypto_shash_setkey(tfm, key, keylen);
    if (ret)
        goto out;

    ret = crypto_shash_init(shash);
    if (ret)
        goto out;

    ret = crypto_shash_update(shash, data, datalen);
    if (ret)
        goto out;

    ret = crypto_shash_final(shash, out);

out:
    kfree(shash);
    crypto_free_shash(tfm);
    return ret;
}

// Truncate function as per RFC 4226 (dynamic truncation)
static u32 truncate(const u8 *hmac_result, size_t hmac_len)
{
    int offset = hmac_result[hmac_len - 1] & 0x0F;
    u32 bin_code = (hmac_result[offset] & 0x7f) << 24 |
                   (hmac_result[offset + 1] & 0xff) << 16 |
                   (hmac_result[offset + 2] & 0xff) << 8 |
                   (hmac_result[offset + 3] & 0xff);
    return bin_code;
}

// POTP generation function
static int generate_potp(u8 *out)
{
    u8 hmac_result[32]; // SHA-256 output size
    u8 data[4 + 4 + 2]; // T (4 bytes), SN (4 bytes), src_addr (2 bytes)
    int T, ret;
    u32 SN_local;
    unsigned short src_addr_local;

    // Prepare data
    T = ((int)ktime_get_real_seconds() - T0) / X;
	if (T != T_prev) {
		T_prev = T;
		SN = 0;
	} else {
		SN++;
		// TODO introdurre massimo sequence number??? non obbligatorio
		//printk(KERN_INFO "POTP: Sequence Number incremented to %d\n", SN);
	}
    SN_local = (u32)SN;
    src_addr_local = src_addr;

    memcpy(data, &T, 4);
    memcpy(data + 4, &SN_local, 4);
    memcpy(data + 8, &src_addr_local, 2);

    ret = hmac_sha256((const u8 *)PSK, strlen(PSK), data, sizeof(data), hmac_result, sizeof(hmac_result));
    if (ret)
        return ret;

    // Truncate to 4 bytes (RFC 4226 dynamic truncation)
    u32 otp = truncate(hmac_result, sizeof(hmac_result));
    memcpy(out, &otp, POTP_LEN);

    return 0;
}

static void example_potp_usage(void)
{
    u8 potp[POTP_LEN];
    int ret, i;

    ret = generate_potp(potp);
    if (ret) {
        pr_err("POTP generation failed: %d\n", ret);
        //return;
    }

    pr_info("Generated POTP: ");
    for (i = 0; i < POTP_LEN; i++)
        pr_cont("%02x ", potp[i]);
    pr_cont("\n");
}
/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
