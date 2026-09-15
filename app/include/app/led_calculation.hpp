#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>

class LEDCaluculation
{
public:
    LEDCaluculation(float range_min, float range_max, uint16_t pixels_num)
        : range_min_(range_min), range_max_(range_max), pixels_num_(pixels_num)
    {
        assert(range_max_ > range_min_ && "range_max must be greater than range_min");
        assert(pixels_num_ > 0 && "pixels_num must be greater than 0");
    }

    uint8_t led_caluculation(float value) const
    {
        value = std::clamp(value, range_min_, range_max_);

        float range = range_max_ - range_min_;
        float ratio = (value - range_min_) / range;

        float pixel_f = ratio * (pixels_num_ - 1);

        return static_cast<uint8_t>(std::round(pixel_f));
    }

private:
    float range_min_;
    float range_max_;
    uint16_t pixels_num_;
};