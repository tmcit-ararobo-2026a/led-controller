#pragma once
#include <array>
#include <cstdint>

#include "app/pixel.hpp"

template <uint16_t PixelNum>
class CommunicationLEDSet
{
public:
    CommunicationLEDSet(uint32_t interval_ms) : interval_ms_(interval_ms) {}

    // get timer
    void setup()
    {
        now_ms = HAL_GetTick();
    }

    void gradually_show_fill(uint8_t r, uint8_t g, uint8_t b, uint32_t time_ms) {}

private:
    std::array<Pixel, PixelNum> pixels_{};
    uint32_t interval_ms_{};
    const uint32_t now_ms{};
};