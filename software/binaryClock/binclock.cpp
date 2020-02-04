#include "binclock.h"
#include "binclockHAL.h"
#include "Arduino.h"
#include "config.h"
#include "wifi.h"
#include <TimeLib.h>

RTC_DS3231 BinaryClock::rtc;

BinaryClock::BinaryClock(): brightness(100), brighttype(STATIC), state(STATES::SHOW_CLK) {
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
        updateDisplay(0x1F,temp,temp);
        break;
      case MANUAL:
        updateDisplay(row[0],row[1],row[2]);
        break;
      case CONF:
      case SET_WIFI:
      case SET_BRIGHTTYPE:
      case SET_DEFAULTBRIGHT:
      case WIFI_UPDATE:
        updateDisplay(state+1-SET_WIFI, temp, 0x3F); //ClockConfig::config.permanent_wifi
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
void BinaryClock::configMode() {
  state = CONF;
  temp = 0x3F;
  effect = BLINK;
}

void BinaryClock::keyPress(SOFTKEY key) {
  switch(state) {
    
    case SHOW_CLK:
      switch(key) {
        case KEY_PLUS:
          state = SET_BRIGHTNESS;
          timeout = 30;
          temp = (brightness<10)?1:brightness/10;
          break;
        case KEY_SET:
          rtcGet();
          state = SET_HOUR;
          temp = 0;
          break;
        default:
        break; 
      }
      break;
      
    case SET_BRIGHTNESS:
      switch(key) {
        case KEY_PLUS:
          if(temp < 9) {
            ++temp;
          }
          break;
        case KEY_MINUS:      
          if(temp > 1) {
            --temp;
          }
          break;
        default:
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
      
    case CONF:
      if(key == KEY_SET) {
        temp = (1 << ClockConfig::config.permanent_wifi);
        state = SET_WIFI;
        effect = NORMAL;
      } break;
    case SET_WIFI:
      switch(key) {
        case KEY_PLUS:
          if(temp < 4)
            temp<<=1;
          break;
        case KEY_MINUS:
          if(temp > 1)
            temp>>=1;
          break;
        case KEY_SET:
          switch(temp) {
            case 1: ClockConfig::config.permanent_wifi=ClockConfig::WL_ALWAYS_OFF; break;
            case 2: ClockConfig::config.permanent_wifi=ClockConfig::WL_SYNC_ON; break;
            case 4: ClockConfig::config.permanent_wifi=ClockConfig::WL_ALWAYS_ON; break;
          }
          state = SET_BRIGHTTYPE;
          temp = 1 << ClockConfig::config.brightness_type;
          ClockConfig::save();
        break;
      }
      break;
      
    case SET_BRIGHTTYPE:
      switch(key) {
        case KEY_PLUS:
          if(temp < 2)
            temp<<=1;
          setBrightnessType(static_cast<enum BRIGHTNESSTYPE>(temp>>1));
          break;
        case KEY_MINUS:
          if(temp > 1)
            temp>>=1;
          setBrightnessType(static_cast<enum BRIGHTNESSTYPE>(temp>>1));
          break;
        case KEY_SET:
          ClockConfig::config.brightness_type = temp>>1;
          state = SET_DEFAULTBRIGHT;
          temp = (ClockConfig::config.default_brightness<10)?1:ClockConfig::config.default_brightness/10;
          ClockConfig::save();
        break;
      }
      break;
      
    case SET_DEFAULTBRIGHT:
      switch(key) {
        case KEY_PLUS:
          if(temp < 9) {
            ++temp;
          }
          break;
        case KEY_MINUS:      
          if(temp > 1) {
            --temp;
          }
          break;
        case KEY_SET:
          ClockConfig::config.default_brightness = temp*10;
          ClockConfig::save();
          WifiESP::power_hard_off();
          WifiESP::power_on(true);
          WifiESP::update();
          state = WIFI_UPDATE;
          effect = HEARTBEAT;
          temp = 0x3F;
      }
      setBrightness(temp*10);
      break;
    case WIFI_UPDATE:
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

void BinaryClock::setAmbientLight(unsigned int val) {
  if(brighttype == AMBIENT) {
    if(val < 125) {
      BinaryClockHAL::setBrightness(8 + brightness/2);
    }
    else if(val < 180) {
      BinaryClockHAL::setBrightness(30 + val/8 + brightness/5);
    }
    else if(val < 500) {
      BinaryClockHAL::setBrightness(45 + val/20 + brightness/5);
    }
    else {
      BinaryClockHAL::setBrightness(52 + val/20 + brightness/5);
    }
  }
}

void BinaryClock::setBrightness(uint8_t val) {
  if(val > 99)
    val = 99;
  brightness = val==99?100:val;
  if(brighttype == STATIC) {
    BinaryClockHAL::setBrightness(val);
  }
}

    
void BinaryClock::rtcGet() {
  setTime(rtc.now().unixtime());
/*  //if synced (year>2019), time in RTC is without daylight saving time adjustment
  if(year()>2019 && ClockConfig::config.summerWinterSwitch && isSummerTime(year(), month(), day(), hour(), ClockConfig::config.timezone)) {
    adjustTime(3600);
  }
  //if not synced, reset year all 10 years - no daylight saving time adjustment
  if(year() == 2010) {
    rtc.adjust(DateTime(0,month(),day(),hour(),minute(),second()));
  }*/
}
void BinaryClock::rtcSet() {
  rtc.adjust(DateTime(now()));
 /* bool summertime = year()>2019 && ClockConfig::config.summerWinterSwitch && BinaryClock::isSummerTime(year(),month(),day(),hour(),ClockConfig::config.timezone);
  rtcSet(summertime);*/
}
void BinaryClock::rtcSet(bool summertime) {
  time_t t = now();
  if(year()>2019 && summertime) {
   t -= 3600;
  }
  rtc.adjust(DateTime((year(t)<2019)?0:year(t),month(t),day(t),hour(t),minute(t),second(t)));
}
void BinaryClock::setDisplayEffect(DISPEFFECT effect) {
  this->effect = effect;
}
boolean BinaryClock::isSummerTime(unsigned int yyyy, unsigned int mnth, unsigned int dd, unsigned int hh, int tzHours) {
// European Daylight Savings Time calculation by "jurs" for German Arduino Forum
// input parameters: "normal time" for year, month, day, hour and tzHours (0=UTC, 1=MEZ)
// return value: returns true during Daylight Saving Time, false otherwise
{
  if (mnth < 3 || mnth > 10) return false; // keine Sommerzeit in Jan, Feb, Nov, Dez
  if (mnth > 3 && mnth < 10) return true; // Sommerzeit in Apr, Mai, Jun, Jul, Aug, Sep
  if ((mnth == 3 && (hh + 24 * dd) >= (1 + tzHours + 24 * (31 - (5 * yyyy / 4 + 4) % 7))) || ((mnth == 10) && (hh + 24 * dd) < (1 + tzHours + 24 * (31 - (5 * yyyy / 4 + 1) % 7))))
    return true;
  else
    return false;
}
}
