/*
 * Copyright (c) 2024-2025 Nuvoton Technology Corp. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TF_PSA_CRYPTO_EXTRA_CONFIG_H
#define TF_PSA_CRYPTO_EXTRA_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Enable M3351 Hardware Crypto Driver in PSA Crypto Core */
#define PSA_CRYPTO_DRIVER_M3351 1

/* Acceleration capabilities */
/* No SHA ACCEL: the SHA engine holds one streaming state, so overlapping hashes
 * (e.g. the entropy accumulator during psa_crypto_init) need the built-in SHA-256. */

#define MBEDTLS_PSA_ACCEL_KEY_TYPE_AES
#define MBEDTLS_PSA_ACCEL_ALG_ECB_NO_PADDING
#define MBEDTLS_PSA_ACCEL_ALG_CBC_NO_PADDING
#define MBEDTLS_PSA_ACCEL_ALG_CBC_PKCS7
#define MBEDTLS_PSA_ACCEL_ALG_CTR

#ifdef __cplusplus
}
#endif

#endif /* TF_PSA_CRYPTO_EXTRA_CONFIG_H */
