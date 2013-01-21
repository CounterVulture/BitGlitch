#include <avr/io.h>
  //WHY DO CPP FILES WORK BUT .c DOESNT
  
  
void setupdigitalIO(){
  DDRD = DDRD | 0b1111100; //set port direction of digital 2-7 to Out
  DDRB = 0b00101000; //set Digital Pin 11/13 to out, rest to in
  //this also sets orts 14 and 15 which are crystals ports
  //does this have an effect?
  
  PCICR |= (1 << PCIE0);//pinchangeinterruptenable pins8-15
  PCMSK0 |= (1 << PCINT0);//pin8 interrupt enable
  PCMSK0 |= (1 << PCINT1);//pin9 interrupt enableUSHURE?
}


