#include <avr/io.h>
#include "Arduino.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


// set prescale 
//TRY USING HIGH PRESACALE
void setADCPrescaler() {
  //to 8. (2mhz clok)
  //ADCSRA |= (0 << ADPS2) & ~(0 << ADPS1) & ~(0 << ADPS0); 
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
  
  // set up the ADC

}

uint16_t adc_read(uint8_t ch)
{
   //ch=ch&0b00000111;  
  //ADMUX |= ch;                // Channel selection
   
  //ADCSRA |= (1<<ADSC);               // Start conversion
  //while(!(ADCSRA & (1<<ADIF)));   // Loop until conversion is complete
  //ADCSRA |= (1<<ADIF);               // Clear ADIF by writing a 1 (this sets the value to 0)
 
  //return(ADC);
  //cbi(ADCSRA,ADEN);
  return analogRead(ch);
}
