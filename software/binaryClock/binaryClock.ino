
#include <Wire.h>
#include "RTClib.h"
#include <TimeLib.h>
#include "binclockHAL.h"
#include "serialInterface.h"
#include "wifi.h"
#include "config.h"
BinaryClock binClock;
SerialInterface interface(&Serial,&binClock);

unsigned long nextDispUpdate = 0;
unsigned long nextTimeUpdateRTC = 0;
unsigned long nextTimeUpdateNTP = 0;
unsigned long nextTimeUpdateESP_off = 0;
unsigned int ambientLight;

bool wifi_config_menu = false;

unsigned int getAmbientLight(unsigned int val) {
  static unsigned int measure[32];
  static uint8_t ptr;
  unsigned int average = 0;
  
  measure[((++ptr)&0x1F)] = val;
  
  for(int i = 0; i < 32; i++) {
    average += measure[i];
  }
  return average/32;
}

void setup() {
  Serial.begin(9600);
  ClockConfig::load();
  BinaryClockHAL::init();
  WifiESP::init(&interface);
  binClock.setBrightnessType(static_cast<enum BinaryClock::BRIGHTNESSTYPE>(ClockConfig::config.brightness_type));
  binClock.setBrightness(ClockConfig::config.default_brightness);
  BinaryClock::rtcGet();

  nextTimeUpdateNTP = millis() + 1000;
  WifiESP::power_hard_off();
  
  //set is pressed at beginning
  if(!digitalRead(A0)) {
    binClock.configMode();
    WifiESP::power_on(true);
    WifiESP::config_menu();
    nextTimeUpdateNTP = millis() + 300000;
  }
  else {
    switch(ClockConfig::config.permanent_wifi) {
      case ClockConfig::WL_ALWAYS_ON: WifiESP::power_on(false); break;
      case ClockConfig::WL_SYNC_ON: /*nextTimeUpdateNTP = now() + 1;*/ break;
      case ClockConfig::WL_ALWAYS_OFF: /*do nothing */ break;
    }
  }
  interface.helloMsg();
}

void loop() {
  interface.readSerial();
  
  if(BinaryClockHAL::getKeyShort(BinaryClockHAL::Key::MINUS)) {
    binClock.keyPress(BinaryClock::SOFTKEY::KEY_MINUS);
  }
  if(BinaryClockHAL::getKeyShort(BinaryClockHAL::Key::PLUS)) {
    binClock.keyPress(BinaryClock::SOFTKEY::KEY_PLUS);
  }  
  if(BinaryClockHAL::getKeyLong(BinaryClockHAL::Key::SET)) {
    binClock.keyPress(BinaryClock::SOFTKEY::KEY_SET);
  }

  //Time Schedule

  //Every 100ms - Update of Display Content and Display Effects
  if(millis() > nextDispUpdate) {
    nextDispUpdate = millis() + 100;
    binClock.displayTick();
    ambientLight = getAmbientLight(analogRead(A7));
    binClock.setAmbientLight(ambientLight);
  }

  //Every 5 minutes - synchronize internal Clock with RTC
  if(millis() > nextTimeUpdateRTC) {
    nextTimeUpdateRTC = millis() + 300000UL;
    BinaryClock::rtcGet();
  }

  if(millis() > nextTimeUpdateNTP) {
    if(WifiESP::ntp_request()) {
      nextTimeUpdateNTP = millis() + 86400000UL;
      nextTimeUpdateESP_off = millis() + 5000UL;
    } else {
      nextTimeUpdateNTP = millis() + 500;
    }
  }
  //Every day at 3AM - synchronize by NTP
 /* if(nextTimeUpdateNTP && now() > nextTimeUpdateNTP) {
    if(ClockConfig::config.permanent_wifi != ClockConfig::WL_ALWAYS_OFF) {
      if(WifiESP::getPowerStat() == WifiESP::POWER_OFF) {
        WifiESP::power_on(false);
        //Shutdown ESP after 10 Seconds, if not responding
        nextTimeUpdateESP_off = millis() + 10000UL;
      }
      if(WifiESP::ntp_request()) {
        /* 
         * NTP request is scheduled at 5 Seconds after 3AM - even if internal clock is a bit to fast, 
         * daylight saving time adjustment over NTP works.
         */
   /*     nextTimeUpdateNTP = 3*3600 + 5 + (elapsedSecsToday(now()) < (3*3600))?previousMidnight(now()):nextMidnight(now());
        nextTimeUpdateESP_off = millis() + 5000UL;
      } else {
        nextTimeUpdateNTP = now() + 1;
      }
    }
  }*/
  
  if(nextTimeUpdateESP_off && millis() > nextTimeUpdateESP_off) {
    nextTimeUpdateESP_off = 0;
    if(ClockConfig::config.permanent_wifi != ClockConfig::WL_ALWAYS_ON)
      WifiESP::power_soft_off();
  }

}
