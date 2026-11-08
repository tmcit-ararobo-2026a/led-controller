# LED Controller

Neopixel WS2813 controller for NHK Robocon 2026.  
An STM32G4 drives the RGB LEDs via DMA.

## Types of LED Strips

The robot is equipped with three LED strips.

- **Front LED Strip (115 pixels):** Shows where objects are, as seen from the robot
- **Rear LED Strip (75 pixels):** Shows the belt drive power
- **Front-Top LED Strip (28 pixels):** Shows the communication status and autopilot status

---

## Front LED

Indicates the positions of target objects based on the robot's localization data.  
The LEDs light up when an object's angle falls within the range between `RANGE_MIN` and `RANGE_MAX`.  
The lit LEDs move according to the object's position relative to the robot.  
If multiple objects fall within the range, their LED colors are blended.  

| Object / State | LED Color | Lighting Effect |
| :--- | :--- | :--- |
| **Flag** | Sky Blue | Solid (All LEDs) |
| **Bucket 1** | Green | Solid (All LEDs) |
| **Bucket 2 & 3** | Red | Solid (All LEDs) |
| **Move Bucket** | Yellow | Solid (All LEDs) |
| **Initial State** | Sky Blue | Solid (All LEDs) |
| **Out of Range** | Red & Sky Blue | Sky blue base, with 15 red LEDs lit at each edge |

---

## Rear LED

Displays the current power output of the belt drive.  
For implementation details on the number of lit LEDs, please refer to:  
`app/include/app/belt_led_set.hpp`

### Status Indicators

| Belt Status | LED Color |
| :--- | :--- |
| **Shot OK** | Green |
| **Shot NOT OK** | Red |

*Note: The number of lit LEDs changes dynamically with the belt power.*

---

## Front-Top LED

Indicates the communication status with the controller and the current autopilot state.

| Communication / Autopilot Status | LED Color | Lighting Effect |
| :--- | :--- | :--- |
| **Controller Comm Disconnected** | Red | Fade / Breathing |
| **Autopilot Standby** | Blue | Fade / Breathing |
| **Autopilot Ready** | Blue | Solid (All LEDs) |
| **Autopilot Tracking** | Blue | Converge & Diverge |
| **Autopilot Moving** | Green | Converge & Diverge |
| **Autopilot Goal** | Green | Solid (All LEDs) |
| **Autopilot Fail** | Red | Solid (All LEDs) |