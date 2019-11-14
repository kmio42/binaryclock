#ifndef SerialInterface_h
#define SerialInterface_h

#include "Arduino.h"
#include "SerialCommands.h"
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

extern bool updaterActive;
extern SerialCommands serial_commands_;
void init_cmds();
void readcmd();

#endif
