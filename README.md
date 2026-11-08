# LED Controller

Neopixel WS2813 Controller for NHK Robocon 2026. 
An STM32G4 controls the RGB LEDs using DMA.

## Types of LED Strips

Our robot is equipped with three LED strips. The number of pixels on each strip is configurable:

- **Front LED Strip (115 pixels):** Indicates the robot's localization status.
- **Rear LED Strip (75 pixels):** Indicates the belt drive power.
- **Front-Top LED Strip (28 pixels):** Indicates the robot's communication and autopilot status.

---

## Front LED

Indicates the object's position based on the robot's localization data.  
The LEDs light up when the object falls within the range between `RANGE_MIN` and `RANGE_MAX`. Colors are dynamically blended based on the detected target.

| Object | LED Color | Lighting Effect |
| :--- | :--- | :--- |
| Flag | Sky Blue | Solid (All LEDs) |
| Bucket 1 | Green | Solid (All LEDs) |
| Bucket 2 & 3 | Red | Solid (All LEDs) |
| Move Bucket | Yellow | Solid (All LEDs) |
| Out of Range | Sky Blue & Red | Solid (15 edge LEDs per color) |

---

## Rear LED

Displays the current power output of the belt drive.  
For implementation details regarding LED count and brightness scaling, please refer to:
`app/include/app/belt_led_set.hpp`

### Status Indicators

| Belt Status | LED Color |
| :--- | :--- |
| **Shot OK** | Green |
| **Shot NOT OK** | Red |

*Note: The number of active LEDs scales dynamically with the belt power.*

---

## Front-Top LED

Indicates the robot's communication status with the controller and the current autopilot phase.

| Communication / Autopilot Status | LED Color | Lighting Effect |
| :--- | :--- | :--- |
| **Controller Comm Disconnected** | Red | Fade / Breathing |
| **Autopilot Standby** | Blue | Fade / Breathing |
| **Autopilot Ready** | Blue | Solid (All LEDs) |
| **Autopilot Tracking** | Green | Converge & Diverge |
| **Autopilot Success** | Green | Solid (All LEDs) |
| **Autopilot Fail** | Red | Solid (All LEDs) |
