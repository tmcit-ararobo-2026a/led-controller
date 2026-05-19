#include "app/app.hpp"

#include "app/neopixel.hpp"
#include "tim.h"

Neopixel strip(&htim15, TIM_CHANNEL_1, 30);

void setup()
{
    strip.clear();
    strip.show();
}
void loop()
{
    strip.gradually_shine(255, 0, 0);
    strip.show();
    strip.fill(255, 0, 0);
    strip.show();
    HAL_Delay(30);
}
extern "C" {

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
{
    strip.pulse_sent_callback(htim);
}
}