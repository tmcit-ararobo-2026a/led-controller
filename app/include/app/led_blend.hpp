#pragma once
#include <algorithm>
#include <array>
#include <cstdint>

#include "app/pixel.hpp"

template <uint16_t PixelNum, class TargetLEDSets>
std::array<Pixel, PixelNum> blend_pixels(const TargetLEDSets& targets)
{
    std::array<Pixel, PixelNum> pixel{};

    auto add_one = [&pixel](const std::array<Pixel, PixelNum>& src) {
        for (uint16_t i = 0; i < PixelNum; ++i) {
            // int -> uint8にすることでオーバーフロー対策
            int r      = static_cast<int>(pixel[i].r) + static_cast<int>(src[i].r);
            int g      = static_cast<int>(pixel[i].g) + static_cast<int>(src[i].g);
            int b      = static_cast<int>(pixel[i].b) + static_cast<int>(src[i].b);
            pixel[i].r = static_cast<uint8_t>(std::clamp(r, 0, 255));
            pixel[i].g = static_cast<uint8_t>(std::clamp(g, 0, 255));
            pixel[i].b = static_cast<uint8_t>(std::clamp(b, 0, 255));
        }
    };

    (add_one(targets.pixels()), ...);

    return pixel;
}