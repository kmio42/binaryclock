
#include <Wire.h>
#include "RTClib.h"
#include <TimeLib.h>
#include "binclockHAL.h"
#include "serialInterface.h"

BinaryClock binClock;
SerialInterface interface(&Serial,&binClock);

unsigned long nextDispUpdate = 0;
unsigned long nextTimeUpdateRTC = 0;
unsigned long nextTimeUpdateNTP = 0;

void setup() {
  delay(100);
  Serial.begin(115200);
  BinaryClockHAL::init();
  BinaryClockHAL::setBrightness(100);
  BinaryClock::rtcGet();
  interface.helloMsg();
  if(timeStatus()!= timeSet) {
    while(1) {
      BinaryClockHAL::display(31,63,63);
    }
  }
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

  if(millis() > nextDispUpdate) {
    nextDispUpdate = millis() + 100;
    binClock.displayTick();
  }
  if(millis() > nextTimeUpdateRTC) {
    BinaryClock::rtcGet();
    nextTimeUpdateRTC = millis() + 300000UL;
  }

}
