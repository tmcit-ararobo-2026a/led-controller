#pragma once
#include <cstddef>
#include <cstdint>

#include "tim.h"

template <std::size_t NumLED>
class Neopixel16bit
{
private:
    TIM_HandleTypeDef* htim_;  // タイマーのハンドラ
    uint32_t channel_;         // タイマーのチャンネル

    // リセット用ゼロ挿入数（300us確保用）
    static constexpr std::size_t RESET_SLOTS       = 500;
    static constexpr std::size_t BIT_LEN_PER_PIXEL = 24;
    static constexpr std::size_t BUFFER_SIZE       = NumLED * BIT_LEN_PER_PIXEL + RESET_SLOTS;

    uint16_t pulse_data_[BUFFER_SIZE];  // 送信用パルス幅データ
    volatile bool data_sent_ = true;    // データが送信済みかどうか

    // パルス幅（タイマーのARR/PSC設定に合わせ変更可能）
    uint16_t high_count_ = 19;  // 1を送信するときのパルス幅
    uint16_t low_count_  = 5;   // 0を送信するときのパルス幅

public:
    /**
     * @brief Neopixelクラスのコンストラクタ
     *
     * @param htim タイマーハンドラ（htim1, htim2, ...）
     * @param channel チャンネル（TIM_CHANNEL_1, TIM_CHANNEL_2, ...）
     * @param high データHIGHを送信する際のCOUNT
     * @param low データLOWを送信する際のCOUNT
     */
    Neopixel16bit(TIM_HandleTypeDef* htim, uint32_t channel, uint16_t high = 19, uint16_t low = 5)
        : htim_(htim), channel_(channel), high_count_(high), low_count_(low)
    {
    }

    /**
     * @brief LEDのデータを送信する
     *
     * @return true: 送信成功, false: 送信失敗
     */
    bool show()
    {
        if (data_sent_) {
            data_sent_ = false;
            HAL_TIM_PWM_Stop_DMA(htim_, channel_);
        } else {
            return false;  // 前回の送信が完了していない
        }
        std::memset(
            &pulse_data_[NumLED * 24], 0, RESET_SLOTS * sizeof(uint16_t)
        );  // リセット部分は0
        HAL_TIM_PWM_Start_DMA(
            htim_, channel_, reinterpret_cast<uint32_t*>(pulse_data_), BUFFER_SIZE
        );
        return true;
    }

    /**
     * @brief PWM信号が送信されたときのコールバック
     *
     * @param htim タイマーハンドラ（htim1, htim2, ...）
     */
    void pulse_sent_callback(TIM_HandleTypeDef* htim)
    {
        if (htim->Instance != htim_->Instance) return;
        data_sent_ = true;
    }

    /**
     * @brief 指定したLEDの色を設定する
     *
     * @param pixel LEDのインデックス（0からnum_pixels-1まで）
     * @param r 赤色成分（0-255）
     * @param g 緑色成分（0-255）
     * @param b 青色成分（0-255）
     */
    void set_pixel_color(std::size_t pixel, uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief 指定した範囲LEDの色を設定する
     *
     * @param start_pixel LEDのインデックスの始まり
     * @param end_pixel LEDのインデックスの終わり
     * @param r 赤色成分（0-255）
     * @param g 緑色成分（0-255）
     * @param b 青色成分（0-255）
     */
    void set_pixel_range(
        std::size_t start_pixel, std::size_t end_pixel, uint8_t r, uint8_t g, uint8_t b
    );

    /**
     * @brief すべてのLEDの色を設定する
     *
     * @param r 赤色成分（0-255）
     * @param g 緑色成分（0-255）
     * @param b 青色成分（0-255）
     */
    void fill(uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief すべてのLEDを消灯する
     */
    void clear()
    {
        std::fill_n(pulse_data_, NumLED * BIT_LEN_PER_PIXEL, low_count_);
    }
};