# User Interface/ manual

## Keys
There are Three different Key actions:
* plus (+)
* minus (-)
* set - long press minus key

The concrete function of keys depend on state.

### General Settings
For General Settings like Wifi-Settings, power off binary clock and press **SET** key during power on.
The display begins to blink, to show configuration mode. Values can be changed by **+** and **-** keys,
confirmed and saved by pressing **SET** long. Latter also will switch to next configuration according to following table.
State is shown in *hour* display as binary number, setting in *minute* display, while in display of *seconds* all leds are on. 

| State | Configuration | Options | Comment |
| ---   | --- | --- | --- | 
|  0    | START         |       - | all leds of minutes and seconds are blinking, <br>state may be bypassed by key bouncing  |
|  1    | WIFI          | 1 - always off,<br> 2 - on for synchronization,<br> 4 - always on | |
|  2    | BRIGHTNESS TYPE | 1 - static,<br> 2 - ambient light adapted |  |
|  3    | BRIGHTNESS VALUE | value between 1 and 10 | default brightness after power on or reset |
|  4    | WIFI update mode | - | blinking of minutes and seconds, <br>sets wifi module in *update* mode <br>(firmware update possible), <br>no further configuration for clock. <br>Leave mode by power down |

The Wifi-module will open a network called "Binaryclock", which can be joined by PC or Smartphone.
Open Browser with IP-Adress **192.168.4.1** to configure Wifi.



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
* `b [value]` - set brightness to value (possible between 0 and 100)
* `t` - print time and date to command line
* `st H:M[:S] [d.m.yyyy] [UTC]` - set time to given value. If Attribute UTC is given, time offset and daylight saving time are considered according to settings. Otherwise time is interpreted as local time.
* `disp row1 row2 row3` - displays arbitrary numbers as binary on display
* `effect [mode]` - set effect mode of display, mode can be:
  * `normal`
  * `blink`
  * `flicker`
  * `heart` (like linux heartbeat trigger for LEDs)
* `set [option]` - show option
* `set [option] [value]` - set option, options can be:
  * `wifi` - `on` *(always on)*, `sync` *(on for synchronization)*, `off` - *(always off)*
  * `bright` - `auto` *(ambient light sensor on)*, `stat` *(static brightness)*
  * `timezone` - integer for time shift in reference to UTC
  * `summerwinter` - `auto` *(automatic switch for daylight saving time)*, `man`*(no automatic switch)*
* `save` save options to EEPROM
* `h` - print help

for background details look at: [Interfaces](Interfaces.md)

## WLAN Interface
If ESP8266 module is connected to a WLAN, it provides an http-server on port 80, to interact with serial command line interface. It provides mDNS, so instead of IP address, the local name: ``binaryclock.local`` can be used.
If opened in browser, it shows up: "CMD-Line Binary-Clock".
The arguments given by HTTP-GET request are converted to a command on serial interface. The answer of clock microcontroller is returned in payload.

For example:
1. **binaryclock.local/?effect=blink**
1. is converted to following serial command by ESP8266:
1. **effect blink**
1. so display of clock begins to blink

* Note: The first key becomes the serial command, all following values (also for other keys) are concatenated by space
