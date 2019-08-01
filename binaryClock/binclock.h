#ifndef BinaryClock_h
#define BinaryClock_h
#include "RTClib.h"

#include "Arduino.h" 

class BinaryClock {
  
public: 
    enum SOFTKEY { KEY_PLUS, KEY_MINUS, KEY_SET };
    enum DISPEFFECT {NORMAL, BLINK, FLICKER, HEARTBEAT};
    enum BRIGHTNESSTYPE {STATIC, AMBIENT};
    BinaryClock();
      
    //INCOMMING SIGNALS
    void displayTick();
    void keyPress(SOFTKEY key);
    void manualMode(uint8_t row[], unsigned int timeout);
    void normalMode();
    
    void setDisplayEffect(DISPEFFECT type);
    
    void setBrightnessType(BRIGHTNESSTYPE type);
    void setBrightness(uint8_t val);
    void setAmbientLight(uint8_t val);
    
    static void rtcSet();
    static void rtcGet();
    static boolean isSummerTime(unsigned int yyyy, uint8_t mnth, uint8_t dd, uint8_t hh, uint8_t tzHours);

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
  enum STATES {SHOW_CLK, SET_HOUR, SET_MINUTE, SET_BRIGHTNESS, MANUAL};
  enum DISPEFFECT effect;
  enum STATES state;
};
#endif
