#include "binclockHAL.h"

uint8_t BinaryClockHAL::leds[6];
uint8_t BinaryClockHAL::trig;
unsigned int BinaryClockHAL::brightness;
unsigned int BinaryClockHAL::frequency;
uint8_t BinaryClockHAL::minbright;
uint8_t BinaryClockHAL::key_state;      // debounced and inverted key state:
uint8_t BinaryClockHAL::key_press;      // key press detect
uint8_t BinaryClockHAL::key_rpt;      // key long press and repeat

//Interrupt Routines for display and key detection
ISR (TIMER1_OVF_vect) {
  static uint8_t col = 1;
  static uint8_t trigger = 60;
  
  
  PORTD = 0;
  DDRB = 0;
  DDRD = 0;

  col++;
  if(col > 5) {
    col = 0;
  }
  DDRB  = (1 << col);
  PORTD = BinaryClockHAL::leds[col];
  DDRD  = BinaryClockHAL::leds[col];

  if(BinaryClockHAL::minbright) {
    PORTD = 0; 
    DDRB = 0;
    DDRD = 0;
  }

  if((--trigger) == 0) {
    trigger=BinaryClockHAL::trig;
    BinaryClockHAL::debounceISR();
  }
}

ISR (TIMER1_COMPB_vect) {
  PORTD = 0;
  DDRB = 0;
  DDRD = 0;
}

static const PROGMEM unsigned int pwm_table[]= {
    60,   64,   69,   75,   81,
    87,   94,  101,  109,  118,
   127,  137,  148,  160,  173,
   186,  201,  217,  234,  252,
   272,  294,  317,  342,  369,
   398,  430,  464,  501,  540,
   583,  629,  678,  732,  790,
   852,  920,  992, 1071, 1155,
  1246, 1345, 1451, 1566, 1689,
  1823, 1967, 2122, 2290, 2471,
  2665
};


void BinaryClockHAL::debounceISR() {
  static uint8_t ct0, ct1, rpt; 
  uint8_t i;
   /*
   * Tastenentprellung nach Peter Dannegger 
   * danni@specs.de
   * siehe auch: http://www.mikrocontroller.net/articles/Entprellung
   */

    i = key_state ^ ~PINC;              // key changed ?
    ct0 = ~( ct0 & i );                 // reset or count ct0
    ct1 = ct0 ^ (ct1 & i);              // reset or count ct1
    i &= ct0 & ct1;                     // count until roll over ?
    key_state ^= i;                     // then toggle debounced state
    key_press |= key_state & i;         // 0->1: key press detect
    if( (key_state & REPEAT_MASK) == 0 )        // check repeat function
      rpt = REPEAT_START;               // start delay
    if( --rpt == 0 ) {
      rpt = REPEAT_NEXT;                        // repeat delay
      key_rpt |= key_state & REPEAT_MASK;
    }
}


void BinaryClockHAL::init() {
  init(1000);
}

static void BinaryClockHAL::init(unsigned int frequency) {
  DDRC &= ~((1 << PC0) | (1 << PC1));
  PORTC |= (1 << PC0) | (1 << PC1);
  //Timer1 => Fast PWM-Mode, Upper value OCR1A without (native) PWM Outputs
  //Prescaler1
  TCCR1A = (1 << WGM10) | (1 << WGM11);
  TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS10); //Prescaler 1
  TCCR1C = 0; 
  TIMSK1 = (1 << TOIE1) | (1 << OCIE1B); //Interrupt Sources ovf, comparematch
  if(frequency < 42) {
    frequency = 42;
  }
  BinaryClockHAL::frequency = frequency;
  OCR1A = F_CPU / frequency / 6; // 6kHz horizontal update rate => 1kHz Display update rate
  
  trig = (int) frequency*6/100;
}

static void BinaryClockHAL::display(uint8_t row1,uint8_t row2,uint8_t row3) {
  uint8_t i=0;
  for(i=0;i<6;i++) {
    leds[i]=((row1&1)<<PD5) | ((row2&1)<<PD6) | ((row3&1)<<PD7);
    row1>>=1;
    row2>>=1;
    row3>>=1;
  }
}

static void BinaryClockHAL::setBrightness(uint8_t brightness) {
    if(brightness > 100) {
      brightness = 100;
    }
    if(brightness < 57) {
      minbright = 1;
    } else {
      minbright = 0;
      OCR1B = pgm_read_word_near(pwm_table + brightness-49);
    }
    BinaryClockHAL::brightness = brightness;
}

static uint8_t BinaryClockHAL::getKeyRpt( uint8_t key_mask )
{
  cli();                                          // read and clear atomic !
  key_mask &= key_rpt;                            // read key(s)
  key_rpt ^= key_mask;                            // clear key(s)
  sei();
  return key_mask;
}

static uint8_t BinaryClockHAL::getKeyShort( uint8_t key_mask ) {
  cli();
  key_mask=~key_state & key_mask;       // read key state and key press atomic !
  key_mask &= key_press;                // read key(s)
  key_press ^= key_mask;  
  sei();
  return key_mask;
}

static uint8_t BinaryClockHAL::getKeyLong( uint8_t key_mask ) {
  cli();
  key_mask &= key_rpt;                  // read key(s)
  key_rpt ^= key_mask;                  // clear key(s)
  key_mask &= key_press;                // read key(s)
  key_press ^= key_mask;   
  sei();
  return key_mask;
}
