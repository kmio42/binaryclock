# Software Architecture

## Needed Libraries

* [RTCLib](https://github.com/adafruit/RTClib)
* [Time](https://github.com/kmio42/Time)
* [SerialCommand](https://github.com/ppedro74/Arduino-SerialCommands)

For ESP8266 Wifi Module:

* [WiFiManager](https://github.com/tzapu/WiFiManager)

## Classes

### Arduino Project (binary.ino)

Setup objects and contains clue logic. It handles event propagation in non-blocking main-loop.

### BinaryClockHAL

This class encapsulates the complete hardware. 
It provides Interface to read out ambient sensor and keys, set brightness and values of LED matrix.
For this purpose it contains mikrocontroller specific Interrupt routines.
All functions of class are static.

### BinaryClock

This class contains the logic of binary clock.
It implements a state machine of what should be displayed and what functions keys have. At moment there are 5 states:

| state | display | trigger |
| ---   |   ---   | --- |
| show clock (*default*) | time | **+** : set brightness, **SET** : set hour |
| set hour | hour | **+** : increment, **-** : decrement, **SET** : set minutes |
| set minutes | minutes | **+** : increment, **-** : decrement, **SET** : store time and return |
| set brighntess | brightness | **+** : increment, **-** decrement, timeout: return |
| manual | arbitrary numbers | any key: return, timeout: return |

Besides that, it creates display effects and manages brightness calculation (BinaryClockHAL needs an absolute brightness value between 0 and 100)
based on ambient light sensor or manual brightness mode.
Furthermore it wraps access to RTC library.

### SerialInterface

Handle incomming commands on serial interface

### ClockConfig

Holds configuration settings, like timezone.
Loading from EEPROM and storing is not yet implemented.

### WifiESP

Encapsulates Access to Wifi module.
