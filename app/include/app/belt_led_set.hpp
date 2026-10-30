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

    struct VelocityBarRange {
        float velocity_threshold;
        uint16_t bar_start;
    };

    static constexpr std::array<VelocityBarRange, 12> kBarRangeTable = {
        {
         {3.2f, 72},  {3.45f, 70}, {3.7f, 68},  {3.95f, 66}, {4.2f, 64},
         {4.45f, 62}, {4.7f, 60},  {4.95f, 58}, {5.2f, 56},  {5.45f, 54},
         {5.7f, 52},  {5.95f, 50}, {6.2f, 48},  {6.45f, 46}, {6.7f, 44},
         {6.95f, 42}, {7.2f, 40},  {7.45f, 38}, {7.7f, 36},
         }
    };

    const std::array<Pixel, PixelNum>& to_pixels() const
    {
        return pixels_;
    }

    void set_bar_color_from_table(float belt_velocity, uint8_t r, uint8_t g, uint8_t b)
    {
        uint16_t start = kBarRangeTable.back().bar_start;  // デフォルトは最後の値(37)

        for (const auto& entry : kBarRangeTable) {
            if (belt_velocity <= entry.velocity_threshold) {
                start = entry.bar_start;
                break;
            }
        }

        // バー部分をまず全部クリア
        for (uint16_t i = bar_start_pixel_; i < PixelNum; i++) {
            pixels_[i].r = 0;
            pixels_[i].g = 0;
            pixels_[i].b = 0;
        }
        // start〜末尾を着色
        for (uint16_t i = start; i < PixelNum; i++) {
            pixels_[i].r = r;
            pixels_[i].g = g;
            pixels_[i].b = b;
        }
    }

private:
    LEDIndexConversion conversion_;
    uint16_t bar_start_pixel_;
    std::array<Pixel, PixelNum> pixels_{};
};