/*
 * SPDX-FileCopyrightText: 2024, Peter Christoffersen
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <sdkconfig.h>
#include <esp_err.h>
#include <esp_heap_caps.h>
#include <driver/mcpwm_types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define POLOLU_DIST_ISR_IRAM_SAFE CONFIG_MCPWM_ISR_IRAM_SAFE
#define POLOLU_DIST_CTRL_FUNC_IN_IRAM CONFIG_MCPWM_CTRL_FUNC_IN_IRAM

#if POLOLU_DIST_ISR_IRAM_SAFE || POLOLU_DIST_CTRL_FUNC_IN_IRAM
#define POLOLU_DIST_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define POLOLU_DIST_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif

#if POLOLU_DIST_ISR_IRAM_SAFE
#define POLOLU_DIST_INTR_ALLOC_FLAG     (ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_IRAM)
#else
#define POLOLU_DIST_INTR_ALLOC_FLAG     (ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_INTRDISABLED)
#endif




typedef struct pololu_dist_t pololu_dist_t;


struct pololu_dist_t {
    mcpwm_cap_channel_handle_t cap_chan;
    pololu_dist_event_cb_t event_cb;
    void *user_data;

    uint32_t cap_timer_res;

    volatile uint32_t sample_start;
    volatile uint32_t sample_end;
};

#ifdef __cplusplus
}
#endif
