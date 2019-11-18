#ifndef BinaryClockCONFIG_h
#define BinaryClockCONFIG_h
#include "Arduino.h"
class ClockConfig {
public:
    enum WL_POWER {
      WL_ALWAYS_OFF = 0,
      WL_SYNC_ON,
      WL_ALWAYS_ON
    };
    static boolean summerWinterSwitch;
    static int8_t timezone;
    static uint8_t permanent_wifi;
    static boolean ambient_brightness;
};
#endif
