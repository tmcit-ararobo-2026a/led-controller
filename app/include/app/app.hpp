#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void setup();
void loop();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include "app/neopixel.hpp"
extern Neopixel strip;
#endif