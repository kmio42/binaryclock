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

### Button

### ESP (WLAN)
