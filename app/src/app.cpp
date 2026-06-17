#include "app/app.hpp"

#include "app/neopixel.hpp"
#include "tim.h"

Neopixel strip(&htim15, TIM_CHANNEL_1, 30);

void setup()
{
    HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);
    strip.clear();
    strip.show();
}
void loop()
{
    HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
    strip.fill(255, 0, 0);
    strip.show();
    HAL_Delay(100);
}

extern "C" {

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
{
    strip.pulse_sent_callback(htim);
}
}