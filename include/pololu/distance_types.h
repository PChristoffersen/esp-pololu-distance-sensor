/*
 * SPDX-FileCopyrightText: 2024, Peter Christoffersen
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define POLOLU_DIST_IRS16A_RES_US 4u
#define POLOLU_DIST_IRS17A_RES_US 1u

#define POLOLU_DIST_IRS16A_1100_15CM  { .res_us = POLOLU_DIST_IRS16A_RES_US, .res_mm = 1u, .range_max_mm = 150u }
#define POLOLU_DIST_IRS16A_1101_35CM  { .res_us = POLOLU_DIST_IRS16A_RES_US, .res_mm = 2u, .range_max_mm = 350u }
#define POLOLU_DIST_IRS16A_1110_50CM  { .res_us = POLOLU_DIST_IRS16A_RES_US, .res_mm = 3u, .range_max_mm = 500u }
#define POLOLU_DIST_IRS16A_1111_100CM { .res_us = POLOLU_DIST_IRS16A_RES_US, .res_mm = 4u, .range_max_mm = 1000u }
#define POLOLU_DIST_IRS17A_0101_130CM { .res_us = POLOLU_DIST_IRS17A_RES_US, .res_mm = 2u, .range_max_mm = 1300u }
#define POLOLU_DIST_IRS17A_1101_300CM { .res_us = POLOLU_DIST_IRS17A_RES_US, .res_mm = 4u, .range_max_mm = 3000u }

#define POLOLU_DIST_4064_50CM  POLOLU_DIST_IRS16A_1110_50CM
#define POLOLU_DIST_4071_130CM POLOLU_DIST_IRS17A_0101_130CM
#define POLOLU_DIST_4079_300CM POLOLU_DIST_IRS17A_1101_300CM


#define POLOLU_DIST_PULSE_MIN_US (1000u)
#define POLOLU_DIST_PULSE_MAX_US (1850u)

typedef struct pololu_dist_t *pololu_dist_handle_t;

typedef struct {
    uint8_t res_us;
    uint8_t res_mm;
    uint16_t range_max_mm;
} pololu_dist_type_t;

typedef struct {
    int gpio_num;
} pololu_dist_config_t;


typedef bool (*pololu_dist_event_cb_t)(pololu_dist_handle_t hndl, uint32_t pulse_us, void *user_data);


#ifdef __cplusplus
}
#endif
