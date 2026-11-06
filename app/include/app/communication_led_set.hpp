#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>

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
            last_r_             = r;
            last_g_             = g;
            last_b_             = b;
            step_count_percent_ = 0;
            dark                = true;
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

            step_count_percent_ += 1;
            if (step_count_percent_ == max_step) {
                last_r_ = last_g_ = last_b_ = 0;
                dark                        = false;
            }
        } else {
            last_r_ += step_r;
            last_g_ += step_g;
            last_b_ += step_b;

            step_count_percent_ -= 1;
            if (step_count_percent_ == 0) {
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

    void converge_and_diverge(uint8_t r, uint8_t g, uint8_t b)
    {
        // 左側から見た中心の位置(偶数: 中心2つのうち左 / 奇数: 真ん中)
        const int32_t last = (static_cast<int32_t>(PixelNum) - 1) / 2;

        int32_t pos;
        int32_t dir;
        if (static_cast<int32_t>(to_half_count) <= last) {
            pos = static_cast<int32_t>(to_half_count);
            dir = 1;
        } else {
            pos = 2 * last - static_cast<int32_t>(to_half_count);
            dir = -1;
        }

        // 全消し
        for (uint16_t i = 0; i < PixelNum; i++) {
            pixels_[i].r = 0;
            pixels_[i].g = 0;
            pixels_[i].b = 0;
        }

        // 尾の後ろから先頭の順に描く(先頭が最後に上書きされる)
        for (int32_t t = TAIL_LENGTH - 1; t >= 0; t--) {
            const int32_t p = pos - dir * t;  // 尾は進行方向と逆側
            if (p < 0 || p > last) {
                continue;
            }

            const uint8_t rr =
                static_cast<uint8_t>(static_cast<uint32_t>(r) * (TAIL_LENGTH - t) / TAIL_LENGTH);
            const uint8_t gg =
                static_cast<uint8_t>(static_cast<uint32_t>(g) * (TAIL_LENGTH - t) / TAIL_LENGTH);
            const uint8_t bb =
                static_cast<uint8_t>(static_cast<uint32_t>(b) * (TAIL_LENGTH - t) / TAIL_LENGTH);

            // 左側
            pixels_[p].r = rr;
            pixels_[p].g = gg;
            pixels_[p].b = bb;

            // 右側(左右対称)
            const int32_t q = static_cast<int32_t>(PixelNum) - 1 - p;
            pixels_[q].r    = rr;
            pixels_[q].g    = gg;
            pixels_[q].b    = bb;
        }

        // カウンタを進めてループさせる
        to_half_count++;
        if (static_cast<int32_t>(to_half_count) > 2 * last) {
            to_half_count = 0;
        }
    }

    const std::array<Pixel, PixelNum>& to_pixels() const
    {
        return pixels_;
    }

private:
    static constexpr int32_t TAIL_LENGTH = 5;  // 尾の長さ(LED数)
    std::array<Pixel, PixelNum> pixels_{};
    uint8_t step_;
    uint32_t timeout_ms_;
    uint16_t step_count_percent_{};
    uint32_t show_ms_{};
    uint32_t to_half_count{};  // converge_and_diverge 用カウンタ
    int last_r_{};
    int last_g_{};
    int last_b_{};
    bool dark = true;
};