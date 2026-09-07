#include "app/app.hpp"

#include "app/neopixel.hpp"
#include "gn10_can/devices/led_server.hpp"
#include "tim.h"

Neopixel strip(&htim15, TIM_CHANNEL_1, 120);

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
}