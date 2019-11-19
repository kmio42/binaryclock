# Hardware
This file is for comments on hardware decisions

## LED-Matrix

The LEDs are arranged in a 3x6 matrix (3 anodes, 6 cathodes).
For each anode two resistors are used.
 
Resistors should be determined to equal brightness impression.
For my LEDs these are the values:

|Color    | night | day |
| ---     | ---   | --- |
| red                 |               |                 |
| green               |               |                 |
| blue                |               |                 |
| magenta             | 40kOhm (60µA) | 428 Ohm (4,9mA) |
| warm-white (yellow) | 82kOhm (34µA) | 857 Ohm (2,8mA) |
| ice-blue (cyan)     | 60kOhm        | 450 Ohm (4,4mA) |

## Options
The clock has a modular design for high customization.
For example the clock should work with hardware keys and RTC (without USB and WLAN)
as well as whith WLAN without USB or hardware keys.
Hardly no Software changes should be neccessary.

### USB
The USB-UART Adapter `ch340` allows a serial interface to the clock over USB.
With this, the Clock can be programmed over USB with Arduino IDE.
Also the clock has a serial command line mode, mentioned in [User Interface](UserInterface.md)

### RTC
The chosen real time clock `ds3231` is a very precise clock ic with support of backup battery.
With this feature the clock can run a long time without readjustment of time, even if clock is
not powered (of course backup battery is needed)

### Ambient Light Sensor
The selected light sensor is `APDS-9007-020`. Quote from datasheet:

> "This device provides a Logarithmic response over a wide dynamic range of 3 lux to 70K lux and has a low sensitivity variation  across  various  light  sources.  It  is  well  suited for applications that operates under high ambient brightness."

The sensor is well suited for the given scenario, especially the detection of very low brightness allows a resilient handling of dark mode at night. As the output of sensor is logarithmical and the PWM tables for LED-Matrix output is logarithmic, mapping from sensor values to output brightness is simple (mostly linear function).

Some example values are given in table below (no strong measurement setting, measured values from 10-bit ADC with 1.1V reference):

| Situation | Value |
| ---       | --- |
| bright sun in summer - direct | 970 |
| bright sun in summer - indoor indirect| 750 |
| cloudy in summer - indoor indirect | 610 |
| desk lamp | 570 |
| ceiling light | 440 |
| dimmed indirect LED light | 130 |
| absolute darkness | 122 |

It is hard to distinguish absolute darkness from low light, unfortunately the required brightness setting differs significantly.

### Button

### ESP (WLAN)
