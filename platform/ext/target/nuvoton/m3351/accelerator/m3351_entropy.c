/*
 * Copyright (c) 2026 Nuvoton Technology Corp. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "psa/crypto.h"
#include "psa/crypto_driver_random.h"
#include "NuMicro.h"

#define TRNG_HEALTH_MSK (TRNG_STS_ESSUT_Msk | TRNG_STS_ESRCT_Msk | TRNG_STS_ESAPT_Msk)

static int m3351_trng_wait_mask(uint32_t mask, uint32_t timeout)
{
    while ((TRNG->STS & mask) != mask) {
        if (timeout-- == 0UL) {
            return -1;
        }
    }
    return 0;
}

/* Same bring-up as BSP TRNG_Open() (NRST needs a falling edge), without module reset */
static int m3351_trng_open(void)
{
    CLK->APBCLK2 |= CLK_APBCLK2_TRNGCKEN_Msk;

    TRNG->CTL |= TRNG_CTL_LDOEN_Msk;
    if (m3351_trng_wait_mask(TRNG_STS_LDORDY_Msk, SystemCoreClock) != 0) {
        return -1;
    }

    TRNG->CTL &= ~TRNG_CTL_NRST_Msk;
    for (volatile uint32_t i = 0; i < 1000UL; i++) {
    }
    TRNG->CTL |= TRNG_CTL_NRST_Msk;
    TRNG->CTL |= TRNG_CTL_TRNGEN_Msk;
    return 0;
}

static int m3351_trng_wait_ready(void)
{
    if ((TRNG->STS & TRNG_STS_TRNGRDY_Msk) == 0UL) {
        /* Start-up health tests may still be running; reopen if never started */
        if (m3351_trng_wait_mask(TRNG_STS_TRNGRDY_Msk, SystemCoreClock / 10UL) != 0) {
            if ((m3351_trng_open() != 0) ||
                (m3351_trng_wait_mask(TRNG_STS_TRNGRDY_Msk, SystemCoreClock) != 0)) {
                return -1;
            }
        }
    }

    return ((TRNG->STS & TRNG_HEALTH_MSK) == TRNG_HEALTH_MSK) ? 0 : -1;
}

/* Direct TRNG START never raises DVIF here; use the TRM "TRNG seeds PRNG" path,
 * reloading the seed from TRNG on every request (RNG_Random sets SEEDRLD). */
static int m3351_trng_read(uint8_t *out, size_t len)
{
    uint32_t words[8];
    size_t i = 0;
    int ret = 0;

    while (i < len) {
        size_t n = len - i;

        if (RNG_Random(words, 8) != 8) {
            ret = -1;
            break;
        }

        if (n > sizeof(words)) {
            n = sizeof(words);
        }
        memcpy(&out[i], words, n);
        i += n;
    }

    memset(words, 0, sizeof(words));
    return ret;
}

/* TRNG entropy source for TF-PSA-Crypto (MBEDTLS_PSA_DRIVER_GET_ENTROPY) */
int mbedtls_platform_get_entropy(psa_driver_get_entropy_flags_t flags,
                                 size_t *estimate_bits,
                                 unsigned char *output, size_t output_size)
{
    if (flags != PSA_DRIVER_GET_ENTROPY_FLAGS_NONE) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (estimate_bits == NULL || output == NULL) {
        return PSA_ERROR_INSUFFICIENT_ENTROPY;
    }

    *estimate_bits = 0;

    if ((m3351_trng_wait_ready() != 0) || (m3351_trng_read(output, output_size) != 0)) {
        return PSA_ERROR_INSUFFICIENT_ENTROPY;
    }

    *estimate_bits = 8 * output_size;
    return 0;
}
