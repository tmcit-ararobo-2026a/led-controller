#pragma once
#include <array>
#include <cstdint>

#include "app/led_index_conversion.hpp"
#include "app/pixel.hpp"

// コンパイル時に配列のサイズ決めたいからtemplete
template <uint16_t PixelNum>
class BeltLEDSet
{
public:
    BeltLEDSet(LEDIndexConversion conversion, uint16_t start_pixel = 0)
        : conversion_(conversion), start_pixel_(start_pixel)
    {
    }

    void set_pixel_color(uint8_t r, uint8_t g, uint8_t b, bool inversion = false)
    {
        for (uint8_t i = start_pixel_; i < index_; i++) {
            pixels_[i].r = r;
            pixels_[i].g = g;
            pixels_[i].b = b;
        }
    }

    Pixel get_pixel_color() const
    {
        return pixels_[index_];
    }

    void set_range(float belt_velocity)
    {
        index_ = conversion_.index_conversion(belt_velocity);
    }

    const std::array<Pixel, PixelNum>& to_pixels() const
    {
        return pixels_;
    }

private:
    std::array<Pixel, PixelNum> pixels_{};
    uint16_t index_ = 0;
    LEDIndexConversion conversion_;
    uint16_t start_pixel_;
};