# Interfaces
 
The clock provides besides the keys two powerful interfaces:
 * USB-Serial Interface 
 * Wifi Interface
 
Both interfaces share same UART of Microcontroller in a way,
that both active at same time would lead to an undeterministic
behaviour. This drawback is due to the fact, that originally
Wifi interface just was intended for NTP time synchronization
and USB Interface for a simple programming method.
 
Nevertheless both interfaces are easily extendible for sending
commands.

## USB-Serial Interface
Features:
* provide an Arduino compatible programming Interface
* provide command line interface to microcontroller - possible commands are mentioned in [User Interface](UserInterface.md)

## Wifi Interface
This interface is realized by powerful ESP8266 module with customized
firmware. Although module is much more powerful than main processor,
it has a strict slave role, so enable pin can only set by microcontroller.

Communication is done in an asynchronous command line style, meaning that
a respond of a command is also wrapped as a command.

Features:
- [x] respond to ntp command with a set time command
- [x] provides an Interface for WiFi settings, realized by access point mode and html configuration page
- [x] configuration mode is enabled by serial command
- [x] provides an interface for microcontroller to send http/https GET requests, e.g. for reading a configuration file
- [x] provides a rudimentary interface for converting serial to wifi by providing a webserver
- [ ] serial interface in both, access point and client mode
- [ ] configuration page offer additional configuration options for binary clock, like switching times for night mode

### API for Developers
The firmware for ESP8266 extension of clock (wifiExtension) supports following commands from serial interface of clock:

* `getip` - return IP address or string *(IP unset)* with `ip `*`IP`* command
* `stat` - return status code of wifi connection (arduino wifi status enum) with `wifistat `*`status`* command
* `conf` - start wifi configuration portal (module is set up as access point)
* `ntp` - get time per NTP and respond with `st `*`time`* command for clock
* `GET `*`URL`* - perform an http get request - return `httpresult `*`result code`* command, pass payload afterwards to serial interface
* `GETS `*`URL`* - perform an https get request - return `httpresult `*`result code`* command, pass payload afterwards to serial 

If module is connected to a WLAN, it provides an http-server on port 80, to interact with serial command line interface. If opened in browser, it shows up: "CMD-Line Binary-Clock".
The arguments given by HTTP-GET request are converted to a command on serial interface. The answer of clock microcontroller is returned in payload.

For example:
1. **192.168.x.x/effect=blink**
1. is converted to following serial command by ESP8266:
1. **effect blink**
1. so display of clock begins to blink

* Note: The first key becomes the serial command, all following values (also for other keys) are concatenated by space

