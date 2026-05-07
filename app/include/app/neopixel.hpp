#pragma once
#include <stdint.h>
#include "tim.h"
#define LED_over_pixel 160
#define LED_under_pixel 120

class Neopixel
{
private:
    TIM_HandleTypeDef *htim;  // タイマーのハンドラ
    uint16_t channel;         // タイマーのチャンネル
    int num_pixels;           // LEDの数
    uint8_t (*pixels)[3];     // LEDの色(GRB:0~255)
    uint16_t *pwm_data;       // PWM波形のパルス幅データ
    bool data_sent = false;   // データが送信済みかどうか
    uint16_t high_pulse = 60; // 1を送信するときのパルス幅
    uint16_t low_pulse = 25;  // 0を送信するときのパルス幅
    uint8_t led_num;
    uint8_t led_num_shine;

    // led記憶用
    bool shine_flag = true;
    bool dark_flag;
    // 定義類
    uint8_t led_r;
    uint8_t led_g;
    uint8_t led_b;

public:
    /**
     * @brief Neopixelクラスのコンストラクタ
     *
     * @param htim タイマーハンドラ（htim1, htim2, ...）
     * @param channel チャンネル（TIM_CHANNEL_1, TIM_CHANNEL_2, ...）
     * @param num_pixels 使用するLEDの最大数
     */
    Neopixel(TIM_HandleTypeDef *htim, uint16_t channel, int num_pixels);

    /**
     * @brief Neopixelクラスのデストラクタ
     */
    ~Neopixel();

    /**
     * @brief LEDのデータを送信する
     *
     * @return true: 送信成功, false: 送信失敗
     */
    bool show();

    /**
     * @brief すべてのLEDを消灯する
     */
    void clear();

    /**
     * @brief すべてのLEDの色を設定する
     *
     * @param r 赤色成分（0-255）
     * @param g 緑色成分（0-255）
     * @param b 青色成分（0-255）
     */
    void fill(uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief 指定したLEDの色を設定する
     *
     * @param pixel LEDのインデックス（0からnum_pixels-1まで）
     * @param r 赤色成分（0-255）
     * @param g 緑色成分（0-255）
     * @param b 青色成分（0-255）
     */
    void set_pixel_color(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief LEDを東京スカイツリーみたいに光らせる
     *
     * @param min_pixel 最初にしたいLED
     * @param pixel_animation_sum どのぐらいのLEDを動かすか、残像を決める
     * @param max_pixel 最後にしたいLED
     * @param r 赤色成分（0-255）
     * @param g 緑色成分（0-255）
     * @param b 青色成分（0-255）
     */
    void flash_sky_tree(uint8_t min_pixel, uint8_t pixel_animation_sum, uint8_t max_pixel, uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief LEDの光を徐々に変えるようにする
     *
     * @param pixel LEDのインデックス（0からnum_pixels-1まで）
     * @param r 赤色成分（0-255）
     * @param g 緑色成分（0-255）
     * @param b 青色成分（0-255）
     */
    void gradually_shine(uint8_t r, uint8_t g, uint8_t b);

    void gradually_dark();

    /**
     * @brief PWM信号が送信されたときのコールバック
     *
     * @param htim タイマーハンドラ（htim1, htim2, ...）
     */
    void pulse_sent_callback(TIM_HandleTypeDef *htim);

    void LED_setup();
};