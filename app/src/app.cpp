#include "app/app.hpp"

// app
#include "app/belt_led_set.hpp"
#include "app/led_blend.hpp"
#include "app/led_information.hpp"
#include "app/localization_led_set.hpp"
#include "app/neopixel16bit.hpp"
#include "app/neopixel32bit.hpp"
#include "app/robot_config.hpp"
// gn10
#include "gn10_can/devices/led_server.hpp"
#include "gn10_stm32_fdcan_driver/can_callback_helper.hpp"
#include "gn10_stm32_fdcan_driver/fdcan_driver.hpp"

// 定数
constexpr uint16_t BEHIND_PIXEL_SUM   = 75;
constexpr uint16_t BEHIND_PIXEL_BREAK = 37;
constexpr uint16_t FRONT_PIXEL_SUM    = 120;

LEDIndexConversion behind_conversion(0, 1.0, BEHIND_PIXEL_BREAK);
LEDIndexConversion belt_conversion(2.0f, 8.0f, BEHIND_PIXEL_SUM);
LEDIndexConversion localization_conversion(-0.7f, 0.7f, FRONT_PIXEL_SUM);

BeltLEDSet<BEHIND_PIXEL_BREAK> behind_fill(behind_conversion);
BeltLEDSet<BEHIND_PIXEL_SUM> belt_power(belt_conversion);
LocalizationLEDSet<FRONT_PIXEL_SUM> localization(localization_conversion);

LEDInformation led_info;
robot_config::command_t led_command;

// gn10_can
gn10_can::drivers::FDCANDriver fdcan1_driver(&hfdcan1);
gn10_can::FDCANBus fdcan1_bus(fdcan1_driver);
gn10_can::devices::LEDServer<LEDInformation> led_server_info(fdcan1_bus, 2);
gn10_can::devices::LEDServer<robot_config::command_t> led_server_command(fdcan1_bus, 1);

// 点滅LED設定
constexpr uint32_t HEARTBEAT_TOGGLE_INTERVAL_MS = 500;
uint32_t heartbeat_last_toggle_time_ms          = 0;

// neopixel
Neopixel16bit<BEHIND_PIXEL_BREAK> behind(&htim15, TIM_CHANNEL_1);
Neopixel32bit<BEHIND_PIXEL_SUM> belt_behind(&htim2, TIM_CHANNEL_4);
Neopixel32bit<FRONT_PIXEL_SUM> localization_front(&htim2, TIM_CHANNEL_1);

/**
 * @brief 一定周期のLEDトグル
 */
void update_heartbeat_led()
{
    const uint32_t now_ms = HAL_GetTick();
    if ((now_ms - heartbeat_last_toggle_time_ms) >= HEARTBEAT_TOGGLE_INTERVAL_MS) {
        heartbeat_last_toggle_time_ms = now_ms;

        HAL_GPIO_TogglePin(LED_2_GPIO_Port, LED_2_Pin);
    }
}

void setup()
{
    fdcan1_driver.init();
    heartbeat_last_toggle_time_ms = HAL_GetTick();
}

void control_led(LEDInformation& led_info)
{
    belt_power.set_range(led_info.belt_velocity, true);

    if (led_info.belt_initialization) {
        behind_fill.set_range(1.0);
        behind_fill.set_pixel_color(120, 0, 0);
        belt_power.set_range(led_info.belt_velocity, true);
        belt_power.set_pixel_color(120, 0, 0, true);
    } else {
        behind_fill.set_range(1.0);
        behind_fill.set_pixel_color(0, 120, 0);
        belt_power.set_range(led_info.belt_velocity, true);
        belt_power.set_pixel_color(0, 120, 0, true);
    }

    behind.set_pixels(behind_fill.to_pixels());
    belt_behind.set_pixels(belt_power.to_pixels());
}

void loop()
{
    if (led_server_info.get_information(led_info)) {
        control_led(led_info);
        HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
    }
}

extern "C" {

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim) {}
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
