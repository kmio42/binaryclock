#ifndef BinaryClock_h
#define BinaryClock_h
#include "RTClib.h"

#include "Arduino.h" 

class BinaryClock {
  
public: 
    enum SOFTKEY { KEY_PLUS, KEY_MINUS, KEY_SET };
    enum DISPEFFECT {NORMAL = 0, BLINK, FLICKER, HEARTBEAT};
    enum BRIGHTNESSTYPE {STATIC=0, AMBIENT};
    BinaryClock();
      
    //INCOMMING SIGNALS
    void displayTick();
    void keyPress(SOFTKEY key);
    void manualMode(uint8_t row[], unsigned int timeout);
    void normalMode();
    void configMode();
    
    void setDisplayEffect(DISPEFFECT type);
    
    void setBrightnessType(BRIGHTNESSTYPE type);
    void setBrightness(uint8_t val);
    void setAmbientLight(unsigned int val);
    
    static void rtcSet();
    static void rtcSet(bool summertime);
    static void rtcGet();
    static boolean isSummerTime(unsigned int yyyy, unsigned int mnth, unsigned int dd, unsigned int hh, int tzHours);

private:
  void updateDisplay(uint8_t row1, uint8_t row2, uint8_t row3);
  unsigned int timeout;
  uint8_t brightnessOffset;
  uint8_t brightness;
  BRIGHTNESSTYPE brighttype;
  uint8_t row[3];
  static RTC_DS3231 rtc;

  int temp;
  int counter;
  enum STATES {SHOW_CLK, SET_HOUR, SET_MINUTE, SET_BRIGHTNESS, MANUAL, CONF,SET_WIFI, SET_BRIGHTTYPE, SET_DEFAULTBRIGHT, WIFI_UPDATE};
  enum DISPEFFECT effect;
  enum STATES state;
};
#endif
