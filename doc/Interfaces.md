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
* provide command line interface to microcontroller (possible commands are mentioned in [Interfaces
(Interfaces.md)

## Wifi Interface
This interface is realized by powerful ESP8366 module with customized
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
