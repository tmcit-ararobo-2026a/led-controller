#pragma once
#include <algorithm>
#include <array>
#include <cstdint>

#include "app/led_index_conversion.hpp"
#include "app/pixel.hpp"

template <uint16_t PixelNum>
class TargetLEDSet
{
public:
    TargetLEDSet(LEDIndexConversion conversion) : conversion_(conversion) {}

    // 呼ばれるたびに、自分の担当分だけクリアして描き直す
    void set_pixel_color(uint8_t r, uint8_t g, uint8_t b)
    {
        pixels_.fill(Pixel{0, 0, 0});

        static constexpr std::array<float, 3> kWeights = {1.0f, 0.6f, 0.25f};

        for (int distance = -static_cast<int>(kWeights.size()) + 1;
             distance <= static_cast<int>(kWeights.size()) - 1;
             ++distance) {
            int target_index = static_cast<int>(index_) + distance;
            if (target_index < 0 || target_index >= static_cast<int>(PixelNum)) {
                continue;
            }
            float weight            = kWeights[std::abs(distance)];
            pixels_[target_index].r = static_cast<uint8_t>(r * weight);
            pixels_[target_index].g = static_cast<uint8_t>(g * weight);
            pixels_[target_index].b = static_cast<uint8_t>(b * weight);
        }
    }

    void set_range(float target_rad)
    {
        index_ = conversion_.index_conversion(target_rad);
    }

    const std::array<Pixel, PixelNum>& pixels() const
    {
        return pixels_;
    }

private:
    std::array<Pixel, PixelNum> pixels_{};
    uint16_t index_ = 0;
    LEDIndexConversion conversion_;
};