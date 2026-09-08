#include <cstdint>

struct led_info {
    /*ベルト直動*/
    bool belt_initialization;
    uint8_t belt_power;

    /*エアー射出*/
    uint8_t air_injection;

    /*電圧*/
    uint8_t control_batt1;
    uint8_t control_batt2;
    uint8_t drive_batt;
} __attribute__((__packed__));
