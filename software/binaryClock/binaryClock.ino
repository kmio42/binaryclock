
#include <Wire.h>
#include "RTClib.h"
#include <TimeLib.h>
#include "binclockHAL.h"
#include "serialInterface.h"
#include "wifi.h"

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
  BinaryClockHAL::init();
  WifiESP::init(&interface);
  BinaryClockHAL::setBrightness(100);
  binClock.setBrightnessType(BinaryClock::BRIGHTNESSTYPE::AMBIENT);
  BinaryClock::rtcGet();
  if(timeStatus() != timeSet) {
    while(1) {
      BinaryClockHAL::display(31,63,63);
    }
  }
  nextTimeUpdateNTP = millis() + 1000;
  WifiESP::power_hard_off();
  //set is pressed at beginning
  if(!digitalRead(A0)) {
    WifiESP::power_on(true);
    WifiESP::config_menu();
    binClock.setDisplayEffect(BinaryClock::DISPEFFECT::BLINK);
    nextTimeUpdateNTP = millis() + 300000;
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
    WifiESP::update();
  }

  if(millis() > nextDispUpdate) {
    nextDispUpdate = millis() + 100;
    binClock.displayTick();
    ambientLight = getAmbientLight(analogRead(A7));
//    Serial.println(ambientLight);
    binClock.setAmbientLight(ambientLight/10 + 20);
  }
  
  if(millis() > nextTimeUpdateRTC) {
    BinaryClock::rtcGet();
    nextTimeUpdateRTC = millis() + 300000UL;
  }

  if(millis() > nextTimeUpdateNTP) {
    if(WifiESP::ntp_request()) {
      nextTimeUpdateNTP = millis() + 86400000UL;
      nextTimeUpdateESP_off = millis() + 5000UL;
    } else {
      nextTimeUpdateNTP = millis() + 500;
    }
  }
  
  if(nextTimeUpdateESP_off && millis() > nextTimeUpdateESP_off) {
    nextTimeUpdateESP_off = 0;
    WifiESP::power_soft_off();
  }

}
