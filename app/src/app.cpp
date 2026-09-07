#include "app/app.hpp"

#include "app/led_info.hpp"
#include "app/neopixel.hpp"
#include "fdcan.h"
#include "gn10_can/devices/led_server.hpp"
#include "gn10_stm32_fdcan_driver/can_callback_helper.hpp"
#include "gn10_stm32_fdcan_driver/fdcan_driver.hpp"
#include "tim.h"

Neopixel strip(&htim15, TIM_CHANNEL_1, 120);
gn10_can::drivers::FDCANDriver fdcan1_driver(&hfdcan1);
gn10_can::FDCANBus fdcan1_bus(fdcan1_driver);
gn10_can::devices::LEDServer<LedInfo> led_server(fdcan1_bus, 2);

void setup()
{
    strip.LED_setup();
}
void loop()
{
    HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
    strip.fill(0, 0, 255);
    strip.show();
    HAL_Delay(1000);
    strip.fill(0, 255, 0);
    strip.show();
    HAL_Delay(1000);
    strip.fill(255, 0, 0);
    strip.show();
    HAL_Delay(1000);
    strip.fill(255, 0, 255);
    strip.show();
    HAL_Delay(1000);
    strip.fill(255, 255, 0);
    strip.show();
    HAL_Delay(1000);
    strip.fill(0, 255, 255);
    strip.show();
    HAL_Delay(1000);
    strip.fill(255, 255, 255);
    strip.show();
    HAL_Delay(1000);
}

extern "C" {

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
{
    strip.pulse_sent_callback(htim);
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