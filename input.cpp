#include <avr/io.h>
#include "Arduino.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

byte portcstatus=0;

// set prescale 
void setADCPrescaler() {
  //to 8. .
  //cbi(ADCSRA,ADPS2) ;
  //sbi(ADCSRA,ADPS1) ;
  //sbi(ADCSRA,ADPS0) ;
  
  //or 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
}

uint16_t adc_read(uint8_t ch)
{
  portcstatus=PORTC;
  return analogRead(ch);
}
