#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>

class LEDIndexConversion
{
public:
    LEDIndexConversion(float range_min, float range_max, uint16_t pixels_num);

    uint8_t index_conversion(float value, bool inversion = false) const;

private:
    float range_min_;
    float range_max_;
    uint16_t pixels_num_;
};