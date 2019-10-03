# User Interface/ manual

## Keys
There are Three different Key actions:
* plus (+)
* minus (-)
* set - long press minus key

The concrete function of keys depend on state.

### General Settings
For General Settings like Wifi-Settings, power off binary clock and press **SET** key during power on.
The display begins to blink, to show configuration mode.
The Wifi-module will open a network called "Binaryclock", which can be joined by PC or Smartphone.
Open Browser with IP-Adress **192.168.4.1** to configute Wifi.

### Time Settings
1. Clock has to be in time-showing state (default)
1. Press **SET** button long
1. Adjust hour with short pressing "**+**" and "**-**" keys
1. Press **SET** button long
1. Adjust minutes with short pressing "**+**" and "**-**" keys
1. Press **SET** button long for returning to time-showing state

### Brightness Settings
There are two variants for brightness setting in clock, *manual mode* and *ambient mode*.
In the latter, brightness is determined by ambient light sensor and following instruction is for fine grained adjustment.
In manual mode, following instructions are for absolute setting of brightness:

1. Clock has to be in time-showing state (default)
1. Press **+** button short
1. Adjust brightness setting with short pressing "**+**" and "**-**" keys
1. Wait 3 seconds to return to time-showing state

## Serial command line Interface
command line interface accepts following commands:
* b [value] - set brightness to value (possible between 0 and 100)
* t - print time and date to command line
* st H:M\[:S\] \[d.m.yyyy\] \[U\]-set time to given value. Attribute U means UTC. Then time offset and daylight saving time is considered according to settings
* disp row1 row2 row3 - displays arbitrary numbers as binary on display
* effect [mode] - set effect mode of display, mode can be:
  * normal
  * blink
  * flicker
  * heart (like linux heartbeat trigger for LEDs)
* h - print help

for background details look at: [Interfaces](Interfaces.md)


