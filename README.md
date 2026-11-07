# LED Controller

Neopixel WS2813 Controller for NHK Robocon 2026.
STM32 G4 controls RGB LED using DMA.

# Types of LED Strips

Our robot has three LED strips.
Show pixels number are variable.

The front LED strip has 115 pixels.  
The front LEDs indicate our robot's localization status.

The behind LED strip has 75 pixels.  
The behind LEDs indicate the belt drive power.

The front-top LED strip has 28 pixels.  
The front-top LEDs indicate our robot's communication status.

## Front LED
The position of an object is indicated using LED, based on robot's localization information.  
The LED lights up when the object settles within the range of   ``RANGE_MIN`` to ``RANGE_MAX``.  
And LED color is blended.

| object | LED color| status | 
| :-: | :-: | :-: |
|flag|sky blue|fill|
|bucket.1|green|fill|
|bucket.2 & 3|red|fill|
|move bucket|yellow|fill|
|out of range|skyblue and red(15 edge pieces)|fill|

## Behind LED 
This LEDs show power of belt drive.  
Please check ``app/include/app/belt_led_set.hpp``.  
The file will show you how many LEDs light up and at what intensity.  
Here,we will tell you two LEDs type.
|belt status|LED color |
| :-: | :-: |
|shot ok | green | 
|shot not ok | red | 

## Front-Top

