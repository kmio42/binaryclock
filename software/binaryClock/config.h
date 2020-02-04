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
    struct conf {
      int8_t timezone;
      boolean summerWinterSwitch;
      uint8_t brightness_type;
      uint8_t permanent_wifi;
      uint8_t default_brightness;
    };
    static struct conf config;
    static void load();
    static void save();
};
#endif
