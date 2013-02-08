#ifndef ANALOGPINS_H
#define ANALOGPINS_H

#include <avr/pgmspace.h>

#define inputPin 0
#define speakerPin 11
  
uint8_t adc_read(uint8_t ch);
void setupanalogIO();

#endif
