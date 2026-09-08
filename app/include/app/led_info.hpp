#include <cstdint>

struct LEDInfo {
    /*ベルト直動*/
    bool belt_initialization;
    uint8_t belt_power;

    /*エアー射出*/
    bool air_injection;  // true:injection false:not injection

    /*電圧*/
    uint8_t control_batt1;
    uint8_t control_batt2;
    uint8_t drive_batt;
} led_info_ __attribute__((__packed__));
