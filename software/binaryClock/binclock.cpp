#include "binclock.h"
#include "binclockHAL.h"
#include "Arduino.h"
#include <TimeLib.h>

RTC_DS3231 BinaryClock::rtc;

BinaryClock::BinaryClock(): state(STATES::SHOW_CLK), brightness(100), brighttype(STATIC){
}

void BinaryClock::displayTick() {
  

    counter++; if(counter > 9) counter = 0;

    switch(state) {
      case SHOW_CLK:
        updateDisplay(hour(),minute(),second());
        break;
      case SET_HOUR:
        updateDisplay(hour() + temp,0,0);
        break;
      case SET_MINUTE:
        updateDisplay(0,minute() + temp,0);
        break;
      case SET_BRIGHTNESS:
        updateDisplay(0,temp+1,temp+1);
        break;
      case MANUAL:
        updateDisplay(row[0],row[1],row[2]);
        break;
      default:
        
      break;
    }

    if(timeout > 0) {
      if(--timeout == 0) {
        state = SHOW_CLK;
      }
    }
}

void BinaryClock::manualMode(uint8_t row[], unsigned int timeout) {
  state = MANUAL;
  this->row[0] = row[0];
  this->row[1] = row[1];
  this->row[2] = row[2];
  this->timeout = timeout * 10;
}
void BinaryClock::normalMode() {
  if(state == MANUAL) {
    state = SHOW_CLK;
  }
}

void BinaryClock::keyPress(SOFTKEY key) {
  switch(state) {
    
    case SHOW_CLK:
      switch(key) {
        case KEY_PLUS:
          state = SET_BRIGHTNESS;
          timeout = 30;
          temp = brightness/10;
          break;
        case KEY_SET:
          rtcGet();
          state = SET_HOUR;
          temp = 0;
          break;  
      }
      break;
      
    case SET_BRIGHTNESS:
      switch(key) {
        case KEY_PLUS:
          if(temp < 10) {
            ++temp;
          }
          break;
        case KEY_MINUS:      
          if(temp > 0) {
            --temp;
          }
          break;
      }
      timeout = 30;
      setBrightness(temp*10);
      break;

    case SET_HOUR:
      switch(key) {
        case KEY_PLUS:
          ++temp;
          if(temp + hour() >= 24) temp -= 24;
          break;
        case KEY_MINUS:
          --temp;
          if(temp + hour() < 0) temp +=24;
          break;
        case KEY_SET:
          adjustTime((long) temp * 3600L);
          temp = 0;
          state = SET_MINUTE;
          break;
      }
      break;
    
    case SET_MINUTE:
      switch(key) {
        case KEY_PLUS:
          ++temp;
          if(temp + minute() >= 60) temp -=60;
          break;
        case KEY_MINUS:
          --temp;
          if(temp + minute() < 0) temp +=60;
          break;
        case KEY_SET:
          adjustTime(temp * 60);
          rtcSet();
          temp = 0;
          state = SHOW_CLK;
      }
      break;
    case MANUAL:
      state = SHOW_CLK;
      break;
  }
  
}

void BinaryClock::updateDisplay(uint8_t row1, uint8_t row2, uint8_t row3) {
  boolean disp_on =  effect == NORMAL;
          disp_on |= effect == BLINK && (counter < 5);
          disp_on |= effect == FLICKER && (counter & 1);
          disp_on |= effect == HEARTBEAT && (counter == 0 || counter == 2);

          if(!disp_on) 
            BinaryClockHAL::display(0,0,0);
          else
            BinaryClockHAL::display(row1,row2,row3);
}

void BinaryClock::setBrightnessType(BRIGHTNESSTYPE type) {
  brighttype = type;
}

void BinaryClock::setAmbientLight(uint8_t val) {
  if(brighttype == AMBIENT) {
    BinaryClockHAL::setBrightness(val + brightness/10);
  }
}

void BinaryClock::setBrightness(uint8_t val) {
  if(val > 100)
    val = 100;
  brightness = val;
  if(brighttype == STATIC) {
    BinaryClockHAL::setBrightness(val);
  }
}

    
void BinaryClock::rtcGet() {
  setTime(rtc.now().unixtime());
}
void BinaryClock::rtcSet() {
  rtc.adjust(DateTime(now()));
}
void BinaryClock::setDisplayEffect(DISPEFFECT effect) {
  this->effect = effect;
}
boolean BinaryClock::isSummerTime(unsigned int yyyy, uint8_t mnth, uint8_t dd, uint8_t hh, uint8_t tzHours) {
// European Daylight Savings Time calculation by "jurs" for German Arduino Forum
// input parameters: "normal time" for year, month, day, hour and tzHours (0=UTC, 1=MEZ)
// return value: returns true during Daylight Saving Time, false otherwise
{
  if (mnth < 3 || mnth > 10) return false; // keine Sommerzeit in Jan, Feb, Nov, Dez
  if (mnth > 3 && mnth < 10) return true; // Sommerzeit in Apr, Mai, Jun, Jul, Aug, Sep
  if (mnth == 3 && (hh + 24 * dd) >= (1 + tzHours + 24 * (31 - (5 * yyyy / 4 + 4) % 7)) || mnth == 10 && (hh + 24 * dd) < (1 + tzHours + 24 * (31 - (5 * yyyy / 4 + 1) % 7)))
    return true;
  else
    return false;
}
}
