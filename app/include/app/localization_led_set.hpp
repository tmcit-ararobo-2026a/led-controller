#pragma once
#include <algorithm>
#include <array>
#include <cstdint>

#include "app/led_index_conversion.hpp"
#include "app/pixel.hpp"

template <uint16_t PixelNum>
class LocalizationLEDSet
{
public:
    LocalizationLEDSet(LEDIndexConversion& conversion) : conversion_(conversion) {}

    void set_pixel_color(uint8_t r, uint8_t g, uint8_t b)
    {
        pixels_.fill(Pixel{0, 0, 0});

        static constexpr std::array<float, 15> bright_ratio = {
            1.0f,
            0.95f,
            0.9f,
            0.85f,
            0.8f,
            0.75f,
            0.7f,
            0.65f,
            0.6f,
            0.55f,
            0.5f,
            0.4f,
            0.3f,
            0.2f,
            0.1f
        };

        for (int distance = -static_cast<int>(bright_ratio.size()) + 1;
             distance <= static_cast<int>(bright_ratio.size()) - 1;
             ++distance) {
            int target_index = static_cast<int>(index_) + distance;

            // 範囲外アクセス防止
            if (target_index < 0 || target_index >= static_cast<int>(PixelNum)) {
                continue;
            }
            float ratio             = bright_ratio[std::abs(distance)];
            pixels_[target_index].r = static_cast<uint8_t>(r * ratio);
            pixels_[target_index].g = static_cast<uint8_t>(g * ratio);
            pixels_[target_index].b = static_cast<uint8_t>(b * ratio);
        }
    }

    void set_range(float target_rad)
    {
        index_ = conversion_.index_conversion(target_rad);
    }

    const std::array<Pixel, PixelNum>& to_pixels() const
    {
        return pixels_;
    }

private:
    std::array<Pixel, PixelNum> pixels_{};
    uint16_t index_ = 0;
    LEDIndexConversion conversion_;
};