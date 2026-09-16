#pragma once
#include <array>
#include <cstdint>

#include "app/led_index_conversion.hpp"
#include "app/pixel.hpp"

template <uint16_t PixelNum>
class BeltLEDSet
{
public:
    BeltLEDSet(LEDIndexConversion& conversion, uint16_t bar_start_pixel = 0)
        : conversion_(conversion), bar_start_pixel_(bar_start_pixel)
    {
    }

    void fill_before_start(uint8_t r, uint8_t g, uint8_t b)
    {
        for (uint16_t i = 0; i < bar_start_pixel_; i++) {
            pixels_[i].r = r;
            pixels_[i].g = g;
            pixels_[i].b = b;
        }
    }

    void set_bar_color(float belt_velocity, uint8_t r, uint8_t g, uint8_t b)
    {
        uint16_t local_index = conversion_.index_conversion(belt_velocity, true);
        uint16_t index       = bar_start_pixel_ + local_index;

        for (uint16_t i = bar_start_pixel_; i < PixelNum; i++) {
            pixels_[i].r = 0;
            pixels_[i].g = 0;
            pixels_[i].b = 0;
        }

        for (uint16_t i = index; i < PixelNum; i++) {
            pixels_[i].r = r;
            pixels_[i].g = g;
            pixels_[i].b = b;
        }
    }

    const std::array<Pixel, PixelNum>& to_pixels() const
    {
        return pixels_;
    }

private:
    uint16_t bar_start_pixel_;
    std::array<Pixel, PixelNum> pixels_{};
    LEDIndexConversion conversion_;
};