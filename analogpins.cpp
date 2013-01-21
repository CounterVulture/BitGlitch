#include <avr/io.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setupanalogIO(){ // set up the ADC
  DDRC = 0b00000000; //set analog pinsto input
  
  DIDR0 |= 00111111; //disable digital input on analog pins. 
  //this should siave energy and should ensure there isn't any 
  //noise in the input due to switching at the digital in
  
  sbi(ADCSRA,ADPS2) ;  //set adc prescaler to 8. (2mhz clok)
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
  //ADCSRA |= (1 << ADPS2) & ~(1 << ADPS1) & ~(1 << ADPS0); 
  ADMUX |= (1<<REFS0);//setref voltage to
  //ADMUX &= ~(1<<REFS1);//+5int
}

uint16_t adc_read(uint8_t ch)
{
  ADMUX &= 0xF0; //Clear the older channel that was read
  ADMUX |= ch; //Defines the new ADC channel to be read
  ADCSRA |= (1<<ADSC); //Starts a new conversion
  while(ADCSRA & (1<<ADSC)); //Wait until the conversion is done
  return ADCW; //Returns the full 10 bit value (ADCH AND ADCL do only part)
}
