#pragma once
#include <cstddef>
#include <cstdint>

#include "app/pixel.hpp"
#include "tim.h"

template <std::size_t NumLED>
class Neopixel32bit
{
private:
    TIM_HandleTypeDef* htim_;  // タイマーのハンドラ
    uint32_t channel_;         // タイマーのチャンネル

    // リセット用ゼロ挿入数（300us確保用）
    static constexpr std::size_t RESET_SLOTS       = 500;
    static constexpr std::size_t BIT_LEN_PER_PIXEL = 24;
    static constexpr std::size_t BUFFER_SIZE       = NumLED * BIT_LEN_PER_PIXEL + RESET_SLOTS;

    uint32_t pulse_data_[BUFFER_SIZE] = {0};  // 送信用パルス幅データ
    volatile bool started_dma_        = false;

    // パルス幅（タイマーのARR/PSC設定に合わせ変更可能）
    uint32_t high_count_ = 19;  // 1を送信するときのパルス幅
    uint32_t low_count_  = 5;   // 0を送信するときのパルス幅

public:
    Neopixel32bit(TIM_HandleTypeDef* htim, uint32_t channel, uint32_t high = 19, uint32_t low = 5)
        : htim_(htim), channel_(channel), high_count_(high), low_count_(low)
    {
    }

    void set_pixels(std::array<Pixel, NumLED> pixels)
    {
        std::size_t index = 0;

        for (std::size_t i = 0; i < NumLED; ++i) {
            // WS2813は GRB 順でデータを発行する
            uint32_t color = (static_cast<uint32_t>(pixels[i].g) << 16) |
                             (static_cast<uint32_t>(pixels[i].r) << 8) |
                             (static_cast<uint32_t>(pixels[i].b));

            for (int bit = 23; bit >= 0; --bit) {
                if (color & (1U << bit)) {
                    pulse_data_[index++] = high_count_;
                } else {
                    pulse_data_[index++] = low_count_;
                }
            }
        }

        while (index < BUFFER_SIZE) {
            pulse_data_[index++] = 0;
        }

        // 初回のみDMAを開始する（Circularモードなので停止命令は不要）
        if (!started_dma_) {
            HAL_TIM_PWM_Start_DMA(
                htim_, channel_, reinterpret_cast<uint32_t*>(pulse_data_), BUFFER_SIZE
            );
            started_dma_ = true;
        }
    }
};