#include <avr/io.h>
  
void setupdigitalIO(){
    DDRD = DDRD | 0b1111100; //set port direction of digital 2-7 to Out
    DDRB = 0b00101000; //set Digital Pin 11/13 to out, rest to in
    //this also sets orts 14 and 15 which are crystals ports
    //does this have an effect?
  
    PCICR |= (1 << PCIE0);//pinchangeinterruptenable pins8-15
    PCMSK0 |= (1 << PCINT0);//pin8 interrupt enable
    PCMSK0 |= (1 << PCINT1);//pin9 interrupt enableUSHURE?
}

void setupPWM(uint16_t samplerate)
{
    ASSR &= ~(_BV(EXCLK) | _BV(AS2));  // Use internal clock 
    TCCR2A |= _BV(WGM21) | _BV(WGM20);  // Set fast PWM mode  
    TCCR2B &= ~_BV(WGM22);  // Set fast PWM mode  
    TCCR2A = (TCCR2A | _BV(COM2A1)) & ~_BV(COM2A0);  // Do non-inverting PWM on pin OC2A 
    TCCR2A &= ~(_BV(COM2B1) | _BV(COM2B0));  // On the Arduino this is pin 11.
    TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);  // No prescaler (p.158)
    TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);  // Set CTC mode (Clear Timer on Compare Match)
    TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));  // Have to set OCR1A *after*, otherwise it gets reset to 0!
    TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);  // No prescaler (p.134)
  
    // Set the compare register (OCR1A).
    OCR1A = F_CPU / samplerate;    // 16e6 / 8000 = 2000
  
    TIMSK1 |= _BV(OCIE1A);  // Enable interrupt when TCNT1 == OCR1A
}
