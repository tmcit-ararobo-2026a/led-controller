#include "app/app.hpp"

#include "app/neopixel.hpp"
#include "tim.h"

Neopixel strip(&htim15, TIM_CHANNEL_1, 30);

void setup()
{
    strip.clear();
    strip.show();
    HAL_Delay(10);

    strip.fill(255, 0, 0);
    strip.show();
}
void loop()
{
    strip.gradually_shine(255, 0, 0);
    strip.show();
    HAL_Delay(30);
}
extern "C" {
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {}
}