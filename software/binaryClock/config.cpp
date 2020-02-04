#include "config.h"
#include <EEPROM.h>
#include "binclock.h"
struct ClockConfig::conf ClockConfig::config = {
  .timezone = 1,
  .summerWinterSwitch = true,
  .brightness_type = BinaryClock::AMBIENT,
  .permanent_wifi = WL_SYNC_ON,
  .default_brightness = 50
};

void ClockConfig::load() {
  EEPROM.get(0,config);
}

void ClockConfig::save() {
  EEPROM.put(0,config);
}
