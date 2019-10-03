# Binary Clock
This project is about an Arduino/Genuino compatible binary clock.

## Main features:
 * Programmable via Arduino IDE and simple USB-cable (hardware compatible with Arduino Nano)
 * Adaption of brightness to ambient brightness
 * Ultra dark night mode for sleeping in dark rooms
 * Adjustment of daylight saving time automatically
 * High precision of time
 * Time keeping with backup battery
 * Time synchronization per WLAN
 * Command line mode via serial console

## Features that turn clock into gadget:
 * Display Effect Modes (blink, flicker, heartbeat)
 * Command for display arbitrary numbers in binary format
 * WLAN module allows IOT operating mode (see [Interfaces](doc/Interfaces.md) for more details)
 
![picture of binary clock](picture.jpg)

## Progress of work
- [x] Core features:
     * display time
     * load time from RTC
     * get time per NTP
     * adjust time
     * adjust brightness
- [x] Serial Command Line for Gadget Mode
- [ ] Selection of Brightness mode (ambient or manual) without reprogramming
- [ ] Configuration of timezone and daylight saving time without reprogramming
- [ ] Adjustment of daylight saving time in offline mode (without WLAN)
