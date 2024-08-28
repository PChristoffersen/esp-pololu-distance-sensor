/*
 * SPDX-FileCopyrightText: 2024, Peter Christoffersen
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <pololu/distance.h>

#include <stdio.h>
#include <inttypes.h>
#include <sdkconfig.h>
#include <esp_check.h>
#include <esp_log.h>
#include <esp_memory_utils.h>
#include <driver/mcpwm_cap.h>

#include <pololu/distance_private.h>

static const char TAG[] = "poldist";

#if POLOLU_DIST_ISR_IRAM_SAFE
#define EVENT_CB_ATTR IRAM_ATTR
#else
#define EVENT_CB_ATTR 
#endif



static bool EVENT_CB_ATTR pololu_dist_capture_event_cb(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_data_t *edata, void *user_data)
{
    bool ret = false;
    pololu_dist_t *dev = user_data;

    if (edata->cap_edge == MCPWM_CAP_EDGE_POS) {
        // store the timestamp when pos edge is detected
        dev->sample_start = edata->cap_value;
        dev->sample_end = dev->sample_start;
    }
    else {
        dev->sample_end = edata->cap_value;
        if (dev->event_cb) {
            uint32_t ticks = dev->sample_end - dev->sample_start;
            uint32_t pulse_us = (uint64_t)ticks * 1000000ull / dev->cap_timer_res;
            ret = dev->event_cb(dev, pulse_us, dev->user_data);
        }
    }
    return ret;
}


static esp_err_t pololu_dist_destroy(pololu_dist_t *dev)
{
    if (dev->cap_chan) {
        mcpwm_del_capture_channel(dev->cap_chan);
    }
    free(dev);
    return ESP_OK;
}


esp_err_t pololu_dist_new(mcpwm_cap_timer_handle_t cap_timer, const pololu_dist_config_t *config, pololu_dist_handle_t *ret_handle)
{
    esp_err_t ret = ESP_OK;
    pololu_dist_t *dev;
    
    dev = heap_caps_calloc(1, sizeof(pololu_dist_t), POLOLU_DIST_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(dev, ESP_ERR_NO_MEM, err, TAG, "no mem for capture timer");

    ESP_LOGI(TAG, "Install capture channel");
    const mcpwm_capture_channel_config_t cap_ch_conf = {
        .gpio_num = config->gpio_num,
        .prescale = 1,
        // capture on both edge
        .flags.neg_edge = true,
        .flags.pos_edge = true,
        // pull down internally
        .flags.pull_up = false,
        .flags.pull_down = true,
    };
    ret = mcpwm_new_capture_channel(cap_timer, &cap_ch_conf, &dev->cap_chan);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Error creating capture channel");

    ret = mcpwm_capture_timer_get_resolution(cap_timer, &dev->cap_timer_res);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Error getting timer resolution");

    *ret_handle = dev;
    return ESP_OK;

err:
    if (dev) {
        pololu_dist_destroy(dev);
    }
    return ret;
}


esp_err_t pololu_dist_register_callback(pololu_dist_handle_t dev, pololu_dist_event_cb_t cb, void *user_data)
{
    esp_err_t ret = ESP_OK;

#if POLOLU_DIST_ISR_IRAM_SAFE
    if (cb) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cb), ESP_ERR_INVALID_ARG, TAG, "on_cap callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif
    dev->user_data = user_data;
    dev->event_cb = cb;

    const mcpwm_capture_event_callbacks_t cbs = {
        .on_cap = (cb!=NULL) ? pololu_dist_capture_event_cb : NULL,
    };
    ret = mcpwm_capture_channel_register_event_callbacks(dev->cap_chan, &cbs, dev);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "Error registering callback");

    return ret;

err:
    dev->event_cb = NULL;
    dev->user_data = NULL;
    return ret;
}

esp_err_t pololu_dist_enable(pololu_dist_handle_t hndl)
{
    ESP_LOGI(TAG, "Enable capture channel");
    return mcpwm_capture_channel_enable(hndl->cap_chan);
}

esp_err_t pololu_dist_disable(pololu_dist_handle_t hndl)
{
    ESP_LOGI(TAG, "Disable capture channel");
    return mcpwm_capture_channel_disable(hndl->cap_chan);
}


