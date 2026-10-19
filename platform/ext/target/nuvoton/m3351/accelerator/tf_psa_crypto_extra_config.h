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

/* No MBEDTLS_PSA_ACCEL_* here: the m3351 driver is not in the generated PSA dispatch,
 * so declaring them would remove the built-in SHA-256/AES and break psa_crypto_init(). */

#ifdef __cplusplus
}
#endif

#endif /* TF_PSA_CRYPTO_EXTRA_CONFIG_H */
