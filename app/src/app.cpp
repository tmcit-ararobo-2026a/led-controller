#include "app/app.hpp"

#include "app/led_information.hpp"
#include "app/neopixel.hpp"
#include "fdcan.h"
#include "gn10_can/devices/led_server.hpp"
#include "gn10_stm32_fdcan_driver/can_callback_helper.hpp"
#include "gn10_stm32_fdcan_driver/fdcan_driver.hpp"
#include "tim.h"

gn10_can::drivers::FDCANDriver fdcan1_driver(&hfdcan1);
gn10_can::FDCANBus fdcan1_bus(fdcan1_driver);
gn10_can::devices::LEDServer<LEDInformation> led_server(fdcan1_bus, 2);

// LED受信構造体
LEDInformation led_info;

Neopixel behind(&htim15, TIM_CHANNEL_1, 120);
Neopixel front(&htim2, TIM_CHANNEL_1, 75);

constexpr uint32_t HEARTBEAT_TOGGLE_INTERVAL_MS = 500;
uint32_t heartbeat_last_toggle_time_ms          = 0;

/**
 * @brief 一定周期のLEDトグル
 */
void update_heartbeat_led()
{
    const uint32_t now_ms = HAL_GetTick();
    if ((now_ms - heartbeat_last_toggle_time_ms) >= HEARTBEAT_TOGGLE_INTERVAL_MS) {
        heartbeat_last_toggle_time_ms = now_ms;

        HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
    }
}

void update_led(LEDInformation& led_info)
{
    /* air */
    if (led_info.air_injection) {
        front.set_pixel_color(0, 120, 0, 0, 0);
    } else {
        front.set_pixel_color(0, 120, 0, 60, 60);
    }

    if (led_info.belt_initialization) {
        if (led_info.belt_velocity <= 2.0f) {
            behind.set_pixel_color(69, 75, 0, 0, 0);
        } else if (led_info.belt_velocity <= 3.0f) {
            behind.set_pixel_color(62, 75, 120, 0, 0);
        } else if (led_info.belt_velocity <= 4.0f) {
            behind.set_pixel_color(57, 75, 120, 0, 0);
        } else if (led_info.belt_velocity <= 5.0f) {
            behind.set_pixel_color(52, 75, 120, 0, 0);
        } else if (led_info.belt_velocity <= 6.0f) {
            behind.set_pixel_color(47, 75, 120, 0, 0);
        } else if (led_info.belt_velocity <= 7.0f) {
            behind.set_pixel_color(42, 75, 120, 0, 0);
        } else {
            behind.set_pixel_color(37, 75, 120, 0, 0);
        }
    } else {
        if (led_info.belt_velocity <= 2.0f) {
            behind.set_pixel_color(69, 75, 0, 0, 0);
        } else if (led_info.belt_velocity <= 3.0f) {
            behind.set_pixel_color(62, 75, 0, 120, 0);
        } else if (led_info.belt_velocity <= 4.0f) {
            behind.set_pixel_color(57, 75, 0, 120, 0);
        } else if (led_info.belt_velocity <= 5.0f) {
            behind.set_pixel_color(52, 75, 0, 120, 0);
        } else if (led_info.belt_velocity <= 6.0f) {
            behind.set_pixel_color(47, 75, 0, 120, 0);
        } else if (led_info.belt_velocity <= 7.0f) {
            behind.set_pixel_color(42, 75, 0, 120, 0);
        } else {
            behind.set_pixel_color(37, 75, 0, 120, 0);
        }
    }

    if (led_info.battery_voltage[0] <= 18.8f) {
        behind.set_pixel_color(0, 11, 120, 0, 0);
    } else if (led_info.battery_voltage[0] <= 19.5f) {
        behind.set_pixel_color(0, 3, 0, 0, 120);
    } else if (led_info.battery_voltage[0] <= 20.5f) {
        behind.set_pixel_color(0, 7, 0, 0, 120);
    } else {
        behind.set_pixel_color(0, 11, 0, 0, 120);
    }

    if (led_info.battery_voltage[1] <= 18.8f) {
        behind.set_pixel_color(12, 24, 120, 0, 0);
    } else if (led_info.battery_voltage[1] <= 19.5f) {
        behind.set_pixel_color(12, 16, 0, 0, 120);
    } else if (led_info.battery_voltage[1] <= 20.5f) {
        behind.set_pixel_color(12, 20, 0, 0, 120);
    } else {
        behind.set_pixel_color(12, 24, 0, 0, 120);
    }

    if (led_info.battery_voltage[2] <= 18.8f) {
        behind.set_pixel_color(25, 37, 120, 0, 0);
    } else if (led_info.battery_voltage[2] <= 19.5f) {
        behind.set_pixel_color(25, 29, 0, 120, 0);
    } else if (led_info.battery_voltage[2] <= 20.5f) {
        behind.set_pixel_color(25, 33, 0, 120, 0);
    } else {
        behind.set_pixel_color(25, 37, 0, 120, 0);
    }
}

void setup()
{
    fdcan1_driver.init();
    behind.LED_setup();
    front.LED_setup();
    heartbeat_last_toggle_time_ms = HAL_GetTick();
}

bool get_command = false;
void loop()
{
    if (!get_command) {
        front.gradually_shine(0, 120, 0, 0, 120);
        front.gradually_dark(0, 120);

        behind.gradually_shine(0, 120, 0, 0, 120);
        behind.gradually_dark(0, 120);
    }

    if (led_server.get_information(led_info)) {
        update_led(led_info);

        get_command = true;
    }
    behind.show();
    front.show();
    HAL_Delay(100);
    update_heartbeat_led();
}

extern "C" {

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
{
    behind.pulse_sent_callback(htim);
    front.pulse_sent_callback(htim);
}
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef* hfdcan, uint32_t RxFifo0ITs)
{
    (void)RxFifo0ITs;
    if (process_fdcan_fifo(hfdcan, &hfdcan1, fdcan1_bus, FDCAN_RX_FIFO0)) return;
}
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef* hfdcan, uint32_t RxFifo1ITs)
{
    (void)RxFifo1ITs;
    if (process_fdcan_fifo(hfdcan, &hfdcan1, fdcan1_bus, FDCAN_RX_FIFO1)) return;
}
}