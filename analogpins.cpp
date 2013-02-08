#include <avr/io.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setupanalogIO(){ // set up the ADC
    DDRC = 0x00; //set analog pinsto input
    DIDR0 |= 0b00111111; //disable digital input on analog pins. 
    //this should save energy and reduce switching noise
  
    sbi(ADCSRA,ADPS2) ;  //set adc prescaler to 8. (2mhz clok)
    cbi(ADCSRA,ADPS1) ;
    cbi(ADCSRA,ADPS0) ;
    
    //ADCSRA |= 0b00000100;
    ADMUX |= (1<<REFS0);//setref voltage to
    ADCSRA |= (1<<ADEN); //enable ADC
    ADCSRA |= (1<<ADSC);//Do an initial conversion because 
    //this one is the slowest and to ensure that everything is up and running
}

uint8_t adc_read(uint8_t ch)
{
    ADMUX &= 0xF0; //Clear the older channel that was read
    ADMUX |= ch; //Defines the new ADC channel to be read
    ADCSRA |= (1<<ADSC); //Starts a new conversion
    while(ADCSRA & (1<<ADSC)); //Wait until the conversion is done
    return ADCW; //Returns the full 10 bit value (ADCH AND ADCL do only part)
    //return ADCH; //returns only ADCH, 8bit value
}
