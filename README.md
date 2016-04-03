#STM32 FunctGen#

This is a basic function generator that uses the DAC on STM32F4 microcontrollers.  This makes use of the floating point math on the F4 
so other MPUs will require some rework (or may not work at all).

This is a GNU ARM Eclipse project.  It was written to work with the stm32 breakout board.

Currently implemented functions are:
* Sine Wave (computed via the CMSIS f32 lookup tables)
* Square Wave
* Triangle Wave
* Sawtooth Wave  

You can control the amplitude (from ~0 to VCC PP) and the frequency (From ~100HZ to 3kHz) using a potentiometer as a voltage divider
read on ADC1 Channels 0 and 1 respectively. 

Definitions for the default pins and ports are in arbwave.h for quick reference or modification.

It's pretty rough at the moment, but I wanted to get it up here for folks who've been watching me make this on livecoding.tv/angst7

## License ##
Licensed under BSD unless otherwise specified in the source code (some code is ST Microcelectronics boilerplate and CMSIS libraries)
See LICENSE for other info.
