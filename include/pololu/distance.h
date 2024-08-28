/*
 * SPDX-FileCopyrightText: 2024, Peter Christoffersen
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <esp_err.h>
#include <driver/mcpwm_types.h>

#include "distance_types.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t pololu_dist_new(mcpwm_cap_timer_handle_t cap_timer, const pololu_dist_config_t *config, pololu_dist_handle_t *ret_handle);

/**
 * Register callback for pulse events (Runs in interrupt context)
 *
 * Return true if a high priority task has been woken up by this function
 */
esp_err_t pololu_dist_register_callback(pololu_dist_handle_t hndl, pololu_dist_event_cb_t cb, void *user_data);

esp_err_t pololu_dist_enable(pololu_dist_handle_t hndl);
esp_err_t pololu_dist_disable(pololu_dist_handle_t hndl);

/**
 * Check if returned pulse is valid
 */
static inline bool pololu_dist_pulse_valid(uint32_t pulse_us)
{
    return pulse_us >= POLOLU_DIST_PULSE_MIN_US && pulse_us < POLOLU_DIST_PULSE_MAX_US;
}

/**
 * Convert pulse to mm
 */
static inline uint32_t pololu_dist_to_mm(const pololu_dist_type_t *sensor_type, uint32_t pulse_us)
{
    return (pulse_us - POLOLU_DIST_PULSE_MIN_US) * sensor_type->res_mm / sensor_type->res_us;
}


#ifdef __cplusplus
}
#endif
