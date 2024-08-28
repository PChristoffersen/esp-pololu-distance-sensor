/*
 * SPDX-FileCopyrightText: 2024, Peter Christoffersen
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <string.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <driver/mcpwm_cap.h>
#include <pololu/distance.h>

#define SENSOR_GPIO 1

static const pololu_dist_type_t SENSOR_TYPE = POLOLU_DIST_4071_130CM;

static TaskHandle_t main_task;
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t data;

static bool IRAM_ATTR sensor_range_event_cb(pololu_dist_handle_t hndl, uint32_t pulse_us, void *user_data)
{
    taskENTER_CRITICAL_ISR(&spinlock);
    data = pulse_us;
    taskEXIT_CRITICAL_ISR(&spinlock);
    
    vTaskNotifyGiveFromISR(main_task, NULL);

    return true;
}


void app_main(void)
{
    mcpwm_cap_timer_handle_t cap_timer;
    pololu_dist_handle_t dist_sensor;

    main_task = xTaskGetCurrentTaskHandle();

    // Configure capture timer
    const mcpwm_capture_timer_config_t cap_conf = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
        .group_id = 0,
    };
    ESP_ERROR_CHECK(mcpwm_new_capture_timer(&cap_conf, &cap_timer));

    // Configure distance sensor
    const pololu_dist_config_t config = {
        .gpio_num = SENSOR_GPIO,
    };
    ESP_ERROR_CHECK(pololu_dist_new(cap_timer, &config, &dist_sensor));
    ESP_ERROR_CHECK(pololu_dist_register_callback(dist_sensor, sensor_range_event_cb, NULL));


    // Start capturing
    ESP_ERROR_CHECK(pololu_dist_enable(dist_sensor));
    
    uint32_t event;
    while (true) {
        event = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (event) {
            uint32_t pulse;

            taskENTER_CRITICAL(&spinlock);
            pulse = data;
            taskEXIT_CRITICAL(&spinlock);

            if (pololu_dist_pulse_valid(pulse)) {
                uint32_t dist_mm = pololu_dist_to_mm(&SENSOR_TYPE, data);
                printf("Range: %lu mm\n", dist_mm);
            }
            else {
                printf("Range: INVAL\n");
            }
        }
    }

    ESP_ERROR_CHECK(pololu_dist_disable(dist_sensor));

}