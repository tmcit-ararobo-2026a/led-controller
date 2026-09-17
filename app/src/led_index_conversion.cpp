#include "app/led_index_conversion.hpp"

LEDIndexConversion::LEDIndexConversion(float range_min, float range_max, uint16_t pixels_num)
    : range_min_(range_min), range_max_(range_max), pixels_num_(pixels_num)
{
    assert(range_max_ > range_min_ && "range_max must be greater than range_min");
    assert(pixels_num_ > 0 && "pixels_num must be greater than 0");
}

uint8_t LEDIndexConversion::index_conversion(float value, bool inversion) const
{
    value = std::clamp(value, range_min_, range_max_);

    float range       = range_max_ - range_min_;
    float ratio       = (value - range_min_) / range;
    float index_float = ratio * (pixels_num_ - 1);

    uint8_t index = static_cast<uint8_t>(std::round(index_float));

    if (inversion) {
        index = (pixels_num_ - 1) - index;
    }

    return index;
}