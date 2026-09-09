#include <cstdint>

struct LEDInfo {
    /*ベルト直動*/
    bool belt_initialization;
    uint8_t belt_power;

    /*エアー射出*/
    bool air_injection;  // true:injection false:not injection

    /*電圧*/
    float logic_battery_voltages1;
    float logic_battery_voltages2;
    uint8_t drive_battery_voltage;
} led_info_ __attribute__((__packed__));
