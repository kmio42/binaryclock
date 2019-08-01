#ifndef BinaryClockHAL_h
#define BinaryClockHAL_h

#include "Arduino.h" 

#define REPEAT_MASK  (1 << PC0) // repeat: key1, key2
#define REPEAT_START  50   // after 500ms
#define REPEAT_NEXT 20    // every 200ms

class BinaryClockHAL {
public:
    enum Key { PLUS = 2, MINUS = 1, SET = 1};
    
    static void init();
    
    static void display(uint8_t row1, uint8_t row2, uint8_t row3);
    static void setBrightness(uint8_t brightness);

    static uint8_t getKeyShort( uint8_t key_mask );
    static uint8_t getKeyLong( uint8_t key_mask );
    static uint8_t getKeyRpt( uint8_t key_mask );
    
    /*
     * don't use following functions
     * only used internally for ISR routines
     */
    static void debounceISR();
    static uint8_t leds[6];
    static unsigned int brightness;
    static uint8_t minbright;
    static uint8_t trig;
 
private:
    BinaryClockHAL();
    static void init(unsigned int frequency);
    
    static unsigned int frequency;
    static uint8_t key_state;
    static uint8_t key_press;      // key press detect
    static uint8_t key_rpt;      // key long press and repeat
};
#endif
