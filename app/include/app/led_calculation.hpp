#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>

class LEDCaluculation
{
public:
    LEDCaluculation(float range_min, float range_max, uint16_t pixels_num);

    uint8_t led_caluculation(float value) const;

private:
    float range_min_;
    float range_max_;
    uint16_t pixels_num_;
};