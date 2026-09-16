#include "app/app.hpp"

/* ---app--- */
// ledの範囲と色の設定をするやつ
#include "app/belt_led_set.hpp"
#include "app/led_blend.hpp"
#include "app/localization_led_set.hpp"
// 構造体共通設定
#include "app/led_information.hpp"
#include "app/robot_config.hpp"
// neopixel
#include "app/neopixel16bit.hpp"
#include "app/neopixel32bit.hpp"
// gn10 submodule
#include "gn10_can/devices/led_server.hpp"
#include "gn10_stm32_fdcan_driver/can_callback_helper.hpp"
#include "gn10_stm32_fdcan_driver/fdcan_driver.hpp"

// 定数
constexpr uint16_t BEHIND_PIXEL_SUM   = 75;
constexpr uint16_t BEHIND_PIXEL_BREAK = 36;
constexpr uint16_t FRONT_PIXEL_SUM    = 120;

LEDIndexConversion belt_conversion(2.0f, 8.0f, BEHIND_PIXEL_SUM - BEHIND_PIXEL_BREAK);
LEDIndexConversion localization_conversion(-0.7f, 0.7f, FRONT_PIXEL_SUM);

BeltLEDSet<BEHIND_PIXEL_SUM> belt_power(belt_conversion, BEHIND_PIXEL_BREAK);

// ターゲットごとに個別のLocalizationLEDSetを用意
LocalizationLEDSet<FRONT_PIXEL_SUM> localization_flag(localization_conversion);
LocalizationLEDSet<FRONT_PIXEL_SUM> localization_bucket1(localization_conversion);
LocalizationLEDSet<FRONT_PIXEL_SUM> localization_bucket2(localization_conversion);
LocalizationLEDSet<FRONT_PIXEL_SUM> localization_bucket3(localization_conversion);
LocalizationLEDSet<FRONT_PIXEL_SUM> localization_move_bucket(localization_conversion);

LEDInformation led_info;
robot_config::command_t led_command;

// gn10_can
gn10_can::drivers::FDCANDriver fdcan1_driver(&hfdcan1);
gn10_can::FDCANBus fdcan1_bus(fdcan1_driver);
gn10_can::devices::LEDServer<LEDInformation> led_server_info(fdcan1_bus, 2);

// neopixel
Neopixel16bit<BEHIND_PIXEL_SUM> belt_behind(&htim15, TIM_CHANNEL_1);
Neopixel32bit<FRONT_PIXEL_SUM> localization_front(&htim2, TIM_CHANNEL_4);

void setup()
{
    fdcan1_driver.init();
}

void control_led_belt(LEDInformation& led_info)
{
    if (!led_info.belt_initialization) {
        belt_power.set_bar_color(led_info.belt_velocity, 120, 0, 0);
        belt_power.fill_before_start(120, 0, 0);
    } else {
        belt_power.set_bar_color(led_info.belt_velocity, 0, 120, 0);
        belt_power.fill_before_start(0, 120, 0);
    }
    belt_behind.set_pixels(belt_power.to_pixels());
}

void control_led_localization(robot_config::command_t& led_command)
{
    localization_flag.set_range(led_command.flag_angle_yaw_rad);
    localization_flag.set_pixel_color(0, 0, 255);  // 旗:青

    localization_bucket1.set_range(led_command.bucket1_angle_yaw_rad);
    localization_bucket1.set_pixel_color(0, 255, 0);  // バケツ1:緑

    localization_bucket2.set_range(led_command.bucket2_angle_yaw_rad);
    localization_bucket2.set_pixel_color(255, 0, 0);  // バケツ2:赤

    localization_bucket3.set_range(led_command.bucket3_angle_yaw_rad);
    localization_bucket3.set_pixel_color(255, 0, 0);  // バケツ3:赤

    localization_move_bucket.set_range(led_command.move_bucket_angle_yaw_rad);
    localization_move_bucket.set_pixel_color(255, 255, 0);  // 移動バケツ:黄色

    auto blended = blend_pixels<FRONT_PIXEL_SUM>(
        localization_flag,
        localization_bucket1,
        localization_bucket2,
        localization_bucket3,
        localization_move_bucket
    );
    localization_front.set_pixels(blended);
}

void loop()
{
    if (led_server_info.get_information(led_info)) {
        control_led_belt(led_info);
        HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
    }

    led_command.bucket1_angle_yaw_rad = 0.3f;
    control_led_localization(led_command);
}

extern "C" {

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef* hfdcan, uint32_t RxFifo0ITs)
{
    (void)RxFifo0ITs;
    HAL_GPIO_TogglePin(LED_2_GPIO_Port, LED_2_Pin);
    if (process_fdcan_fifo(hfdcan, &hfdcan1, fdcan1_bus, FDCAN_RX_FIFO0)) return;
}
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef* hfdcan, uint32_t RxFifo1ITs)
{
    (void)RxFifo1ITs;
    HAL_GPIO_TogglePin(LED_2_GPIO_Port, LED_2_Pin);
    if (process_fdcan_fifo(hfdcan, &hfdcan1, fdcan1_bus, FDCAN_RX_FIFO1)) return;
}
}