#pragma once
#include <algorithm>
#include <array>
#include <cstdint>

#include "app/pixel.hpp"

template <uint16_t PixelNum>
class CommunicationLEDSet
{
public:
    CommunicationLEDSet(uint8_t step, uint32_t timeout_ms) : step_(step), timeout_ms_(timeout_ms)
    {
        show_ms_ = HAL_GetTick();
    }

    void gradually_show_fill(uint8_t r, uint8_t g, uint8_t b, uint8_t percent)
    {
        if (percent == 0 || percent > 100) return;

        const uint32_t now_ms = HAL_GetTick();

        // timeoutしたら設定した色に上書き
        if (now_ms - show_ms_ > timeout_ms_) {
            last_r_     = r;
            last_g_     = g;
            last_b_     = b;
            step_count_ = 0;
            dark        = true;
        }

        // 指定された割合ずつ暗くしていく
        int step_r = r * percent / 100;
        int step_g = g * percent / 100;
        int step_b = b * percent / 100;

        int max_step = 100 / percent;

        if (dark) {
            last_r_ -= step_r;
            last_g_ -= step_g;
            last_b_ -= step_b;

            step_count_ += 1;
            if (step_count_ == max_step) {
                last_r_ = last_g_ = last_b_ = 0;
                dark                        = false;
            }
        } else {
            last_r_ += step_r;
            last_g_ += step_g;
            last_b_ += step_b;

            step_count_ -= 1;
            if (step_count_ == 0) {
                last_r_ = r;
                last_g_ = g;
                last_b_ = b;
                dark    = true;
            }
        }

        // under over阻止
        last_r_ = std::clamp(last_r_, 0, static_cast<int>(r));
        last_g_ = std::clamp(last_g_, 0, static_cast<int>(g));
        last_b_ = std::clamp(last_b_, 0, static_cast<int>(b));

        fill_show(last_r_, last_g_, last_b_);
        show_ms_ = HAL_GetTick();
    }

    void fill_show(uint8_t r, uint8_t g, uint8_t b)
    {
        for (uint16_t i = 0; i < PixelNum; i++) {
            pixels_[i].r = r;
            pixels_[i].g = g;
            pixels_[i].b = b;
        }
    };

private:
    std::array<Pixel, PixelNum> pixels_{};
    uint8_t step_;
    uint32_t timeout_ms_;
    uint16_t step_count_{};
    uint32_t show_ms_{};
    int last_r_{};
    int last_g_{};
    int last_b_{};
    bool dark = true;
};