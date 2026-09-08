#include "app/app.hpp"

#include "app/led_info.hpp"
#include "app/neopixel.hpp"
#include "fdcan.h"
#include "gn10_can/devices/led_server.hpp"
#include "gn10_stm32_fdcan_driver/can_callback_helper.hpp"
#include "gn10_stm32_fdcan_driver/fdcan_driver.hpp"
#include "tim.h"

gn10_can::drivers::FDCANDriver fdcan1_driver(&hfdcan1);
gn10_can::FDCANBus fdcan1_bus(fdcan1_driver);

Neopixel strip1(&htim15, TIM_CHANNEL_1, 120);

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

void setup()
{
    fdcan1_driver.init();
    strip1.LED_setup();
    heartbeat_last_toggle_time_ms = HAL_GetTick();
}

void loop()
{
    strip1.show();
    update_heartbeat_led();
}

extern "C" {

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
{
    strip1.pulse_sent_callback(htim);
}
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef* hfdcan, uint32_t RxFifo0ITs)
{
    (void)RxFifo0ITs;
    if (process_fdcan_fifo(hfdcan, &hfdcan1, fdcan1_bus, FDCAN_RX_FIFO0)) return;
}
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef* hfdcan, uint32_t RxFifo1ITs)
{
    (void)RxFifo1ITs;
    if (process_fdcan_fifo(hfdcan, &hfdcan1, fdcan1_bus, FDCAN_RX_FIFO0)) return;
}
}