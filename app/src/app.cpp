#include "app/app.hpp"

/* ---app--- */
// ledの範囲と色の設定をするやつ
#include "app/belt_led_set.hpp"
#include "app/communication_led_set.hpp"
#include "app/led_blend.hpp"
#include "app/localization_led_set.hpp"
// 構造体共通設定
#include "app/led_information.hpp"
// neopixel
#include "app/neopixel16bit.hpp"
#include "app/neopixel32bit.hpp"
// gn10 submodule
#include "gn10_can/devices/led_server.hpp"
#include "gn10_stm32_fdcan_driver/can_callback_helper.hpp"
#include "gn10_stm32_fdcan_driver/fdcan_driver.hpp"

// 定数
constexpr uint16_t REAR_PIXEL_SUM      = 75;
constexpr uint16_t REAR_PIXEL_BREAK    = 37;
constexpr uint16_t FRONT_PIXEL_SUM     = 115;
constexpr uint16_t FRONT_TOP_PIXEL_SUM = 28;

constexpr float RANGE_MIN = -127.0f;
constexpr float RANGE_MAX = 127.0f;

LEDIndexConversion belt_conversion(2.0f, 8.0f, REAR_PIXEL_SUM - REAR_PIXEL_BREAK);
LEDIndexConversion localization_conversion(RANGE_MIN, RANGE_MAX, FRONT_PIXEL_SUM);

BeltLEDSet<REAR_PIXEL_SUM> belt_power(belt_conversion, REAR_PIXEL_BREAK);
CommunicationLEDSet<FRONT_TOP_PIXEL_SUM> communication_led(10, 100000);

// ターゲットごとに個別のLocalizationLEDSetを用意
LocalizationLEDSet<FRONT_PIXEL_SUM> localization_flag(localization_conversion);
LocalizationLEDSet<FRONT_PIXEL_SUM> localization_bucket1(localization_conversion);
LocalizationLEDSet<FRONT_PIXEL_SUM> localization_bucket2(localization_conversion);
LocalizationLEDSet<FRONT_PIXEL_SUM> localization_bucket3(localization_conversion);
LocalizationLEDSet<FRONT_PIXEL_SUM> localization_move_bucket(localization_conversion);

LEDInformation led_info;

// gn10_can
gn10_can::drivers::FDCANDriver fdcan1_driver(&hfdcan1);
gn10_can::FDCANBus fdcan1_bus(fdcan1_driver);
gn10_can::devices::LEDServer<LEDInformation> led_server_info(fdcan1_bus, 2);

// neopixel
Neopixel16bit<REAR_PIXEL_SUM> belt_behind(&htim15, TIM_CHANNEL_1);
Neopixel32bit<FRONT_PIXEL_SUM> localization_front(&htim2, TIM_CHANNEL_4);
Neopixel32bit<FRONT_TOP_PIXEL_SUM> auto_communication_up(&htim2, TIM_CHANNEL_2);

// callback flag
volatile bool communication = false;
volatile bool timer_100hz_triggered;

bool is_in_range(float value, float min, float max)
{
    return value >= min && value <= max;
}

void control_led_localization(LEDInformation& localization)
{
    bool any_in_range = false;

    if (is_in_range(localization.flag_angle_yaw, RANGE_MIN, RANGE_MAX)) {
        localization_flag.set_range(localization.flag_angle_yaw);
        localization_flag.set_pixel_color(0, 0, 120);
        any_in_range = true;
    } else {
        localization_flag.set_pixel_color(0, 0, 0);
    }

    if (is_in_range(localization.bucket1_angle_yaw, RANGE_MIN, RANGE_MAX)) {
        localization_bucket1.set_range(localization.bucket1_angle_yaw);
        localization_bucket1.set_pixel_color(0, 120, 0);
        any_in_range = true;
    } else {
        localization_bucket1.set_pixel_color(0, 0, 0);
    }

    if (is_in_range(localization.bucket2_angle_yaw, RANGE_MIN, RANGE_MAX)) {
        localization_bucket2.set_range(localization.bucket2_angle_yaw);
        localization_bucket2.set_pixel_color(120, 0, 0);
        any_in_range = true;
    } else {
        localization_bucket2.set_pixel_color(0, 0, 0);
    }

    if (is_in_range(localization.bucket3_angle_yaw, RANGE_MIN, RANGE_MAX)) {
        localization_bucket3.set_range(localization.bucket3_angle_yaw);
        localization_bucket3.set_pixel_color(120, 0, 0);
        any_in_range = true;
    } else {
        localization_bucket3.set_pixel_color(0, 0, 0);
    }

    if (is_in_range(localization.move_bucket_angle_yaw, RANGE_MIN, RANGE_MAX)) {
        localization_move_bucket.set_range(localization.move_bucket_angle_yaw);
        localization_move_bucket.set_pixel_color(120, 120, 0);
        any_in_range = true;
    } else {
        localization_move_bucket.set_pixel_color(0, 0, 0);
    }

    auto blended = blend_pixels<FRONT_PIXEL_SUM>(
        localization_flag,
        localization_bucket1,
        localization_bucket2,
        localization_bucket3,
        localization_move_bucket
    );

    if (localization.flag_angle_yaw == INT8_MIN && localization.bucket1_angle_yaw == INT8_MIN &&
        localization.bucket2_angle_yaw == INT8_MIN && localization.bucket3_angle_yaw == INT8_MIN &&
        localization.move_bucket_angle_yaw == INT8_MIN) {
        for (uint16_t i = 0; i < FRONT_PIXEL_SUM; i++) {
            blended[i].r = 0;
            blended[i].g = 120;
            blended[i].b = 120;
        }
    }

    // 全部範囲外だったときだけ、端15個ずつを赤で上書き
    if (!any_in_range) {
        for (uint16_t i = 0; i < 15; i++) {
            blended[i].r = 120;
            blended[i].g = 0;
            blended[i].b = 0;

            blended[FRONT_PIXEL_SUM - 1 - i].r = 120;
            blended[FRONT_PIXEL_SUM - 1 - i].g = 0;
            blended[FRONT_PIXEL_SUM - 1 - i].b = 0;
        }
    }

    localization_front.set_pixels(blended);
}

void control_led_belt(LEDInformation& led_info)
{
    if (!led_info.belt_initialization) {
        belt_power.set_bar_color_from_table(led_info.belt_velocity, 120, 0, 0);
        belt_power.fill_before_start(120, 0, 0);
    } else {
        belt_power.set_bar_color_from_table(led_info.belt_velocity, 0, 120, 0);
        belt_power.fill_before_start(0, 120, 0);
    }
    belt_behind.set_pixels(belt_power.to_pixels());
}

void control_led_communication(LEDInformation& led_info)
{
    if (led_info.communication_status) {
        communication = true;
    } else {
        communication = false;
    }
    if (communication) {
        if (led_info.navigation_status == robot_config::NavigationStatus::Stanby) {
            communication_led.gradually_show_fill(0, 0, 180, 10);  // blue gradually
        } else if (led_info.navigation_status == robot_config::NavigationStatus::Ready) {
            communication_led.fill_show(0, 0, 180);  // blue
        } else if (led_info.navigation_status == robot_config::NavigationStatus::Tracking) {
            communication_led.converge_and_diverge(0, 0, 180);
        } else if (led_info.navigation_status == robot_config::NavigationStatus::Moving) {
            communication_led.converge_and_diverge(0, 180, 0);
        } else if (led_info.navigation_status == robot_config::NavigationStatus::Goal) {
            communication_led.fill_show(0, 180, 0);
        } else if (led_info.navigation_status == robot_config::NavigationStatus::Fail) {
            communication_led.fill_show(255, 0, 0);
        }
    }

    auto_communication_up.set_pixels(communication_led.to_pixels());
}

void timer_100hz_process()
{
    if (!communication) {
        communication_led.gradually_show_fill(255, 0, 0, 5);
    }
}

void setup()
{
    fdcan1_driver.init();
}

void loop()
{
    if (led_server_info.get_information(led_info)) {
        control_led_belt(led_info);
        control_led_localization(led_info);
        control_led_communication(led_info);
        HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
    }

    if (timer_100hz_triggered) {
        timer_100hz_process();
        timer_100hz_triggered = false;
    }
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    // 100Hz
    if (htim->Instance == TIM7) {
        timer_100hz_triggered = true;
    }
}
}
