#include "app/neopixel_32bit.hpp"

// 徐々に暗くなるプログラムも組む予定です

Neopixel32bit::Neopixel32bit(TIM_HandleTypeDef* htim, uint16_t channel, int num_pixels)
    : htim(htim), channel(channel), num_pixels(num_pixels)
{
    pixels   = new uint8_t[num_pixels][3];
    pwm_data = new uint32_t[num_pixels * 24 + 500];
    for (int i = 0; i < num_pixels; i++) {
        pixels[i][0] = 0;  // G
        pixels[i][1] = 0;  // R
        pixels[i][2] = 0;  // B
    }
    data_sent = true;
}

Neopixel32bit::~Neopixel32bit()
{
    delete[] pixels;
    delete[] pwm_data;
}

bool Neopixel32bit::show()
{
    if (data_sent) {
        data_sent = false;  // 送信が完了したらフラグをリセット
        HAL_TIM_PWM_Stop_DMA(htim, channel);
    } else {
        return false;  // 前回の送信が完了していない場合はfalseを返す
    }
    uint32_t index = 0;
    uint32_t color;

    for (int i = 0; i < num_pixels; i++) {
        color = (pixels[i][0] << 16) | (pixels[i][1] << 8) |
                pixels[i][2] << 0;  // GRB順で24ビットのカラー値を作成
        for (int i = 23; i >= 0; i--) {
            if (color & (1 << i)) {
                pwm_data[index] = high_pulse;  // 1のビットは高いパルス
            } else {
                pwm_data[index] = low_pulse;  // 0のビットは低いパルス
            }
            index++;
        }
    }

    for (int i = 0; i < 500; i++) {
        pwm_data[index++] = 0;  // 500個の0パルスを追加
    }

    HAL_TIM_PWM_Start_DMA(htim, channel, (uint32_t*)pwm_data, index);
    return true;
}

void Neopixel32bit::clear()
{
    for (uint8_t i = 0; i < num_pixels; i++) {
        pixels[i][0] = 0;
        pixels[i][1] = 0;
        pixels[i][2] = 0;
    }
}

void Neopixel32bit::fill(uint8_t r, uint8_t g, uint8_t b)
{
    for (uint8_t i = 0; i < num_pixels; i++) {
        pixels[i][0] = g;
        pixels[i][1] = r;
        pixels[i][2] = b;
    }
}

void Neopixel32bit::set_pixel_color(
    uint8_t min_pixel, uint8_t max_pixel, uint8_t r, uint8_t g, uint8_t b
)
{
    if (max_pixel < num_pixels && min_pixel <= max_pixel) {
        for (uint8_t i = min_pixel; i <= max_pixel; i++) {
            pixels[i][0] = g;
            pixels[i][1] = r;
            pixels[i][2] = b;
        }
    }
}

void Neopixel32bit::pulse_sent_callback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == this->htim->Instance) {
        data_sent = true;
    }
}

void Neopixel32bit::flash_sky_tree(
    uint8_t min_pixel,
    uint8_t pixel_animation_sum,
    uint8_t max_pixel,
    uint8_t r,
    uint8_t g,
    uint8_t b,
    bool reverse
)
{
    if (!reverse) {
        // ---------- 正方向（元の実装のまま） ----------
        if (max_pixel + pixel_animation_sum == led_num) {
            led_num = min_pixel;
            set_pixel_color(max_pixel, max_pixel, 0, 0, 0);
        }

        fill(r / 80, g / 80, b / 80);

        if (min_pixel <= led_num && led_num <= max_pixel + pixel_animation_sum) {
            int div = 1;
            for (int pixel_num = 0;
                 min_pixel <= led_num - pixel_num && pixel_num <= pixel_animation_sum;
                 pixel_num++) {
                int tail_max = led_num - pixel_num;
                int tail_min = (tail_max >= 3) ? tail_max - 3 : 0;
                if (tail_min < min_pixel) tail_min = min_pixel;

                set_pixel_color((uint8_t)tail_min, (uint8_t)tail_max, r / div, g / div, b / div);
                div++;
            }
        }

        led_num++;
    } else {
        // ---------- 負方向（反転版） ----------
        // 折り返し判定：先頭が min_pixel より animation_sum 分手前まで来たら max_pixel に戻す
        if (led_num < (int)min_pixel - (int)pixel_animation_sum) {
            led_num = max_pixel;
            set_pixel_color(min_pixel, min_pixel, 0, 0, 0);
        }

        fill(r / 80, g / 80, b / 80);

        if (led_num <= max_pixel && led_num >= (int)min_pixel - (int)pixel_animation_sum) {
            int div = 1;
            for (int pixel_num = 0;
                 led_num + pixel_num <= max_pixel && pixel_num <= pixel_animation_sum;
                 pixel_num++) {
                int tail_min = led_num + pixel_num;
                int tail_max = (tail_min + 3 <= max_pixel) ? tail_min + 3 : max_pixel;

                set_pixel_color((uint8_t)tail_min, (uint8_t)tail_max, r / div, g / div, b / div);
                div++;
            }
        }

        led_num--;
    }
}

void Neopixel32bit::set_animation_start(int start_value)
{
    led_num = start_value;
}

void Neopixel32bit::gradually_shine(
    uint8_t min_pixel, uint8_t max_pixel, uint8_t r, uint8_t g, uint8_t b
)
{
    if (shine_flag) {
        if (led_r + 10 >= r) {
            led_r = r;
        } else {
            led_r = led_r + 10;
        }

        if (led_g + 10 >= g) {
            led_g = g;
        } else {
            led_g = led_g + 10;
        }

        if (led_b + 10 >= b) {
            led_b = b;
        } else {
            led_b = led_b + 10;
        }

        // flag管理
        if (led_r == r && led_g == g && led_b == b) {
            shine_flag = false;
            dark_flag  = true;
        }

        // 光らせる（範囲指定）
        set_pixel_color(min_pixel, max_pixel, led_r, led_g, led_b);
    }
}

void Neopixel32bit::gradually_dark(uint8_t min_pixel, uint8_t max_pixel)
{
    if (dark_flag) {
        if (0 <= led_r - 10) {
            led_r = led_r - 10;
        } else {
            led_r = 0;
        }
        if (0 <= led_g - 10) {
            led_g = led_g - 10;
        } else {
            led_g = 0;
        }
        if (0 <= led_b - 10) {
            led_b = led_b - 10;
        } else {
            led_b = 0;
        }

        // flag管理
        if (led_r == 0 && led_g == 0 && led_b == 0) {
            shine_flag = true;
            dark_flag  = false;
        }

        // 光らせる（範囲指定）
        set_pixel_color(min_pixel, max_pixel, led_r, led_g, led_b);
    }
}

void Neopixel32bit::LED_setup()
{
    for (int i = 0; i < num_pixels; i++) {
        pixels[i][0] = 0;  // G
        pixels[i][1] = 0;  // R
        pixels[i][2] = 0;  // B
    }

    int pwm_len = num_pixels * 24 + 500;
    for (int i = 0; i < pwm_len; i++) {
        pwm_data[i] = 0;
    }

    shine_flag    = true;
    dark_flag     = false;
    led_num       = 0;
    led_num_shine = 0;
    led_r = led_g = led_b = 0;

    show();
}
