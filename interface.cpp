#include "interface.h"
#include "analogpins.h"
#include <avr/io.h>


//IMPORT THIS INTO ANALOGPINS?
int interfaceClock;
short samplerate;
short bitdepth;


void updateInterface() {
    samplerate = adc_read(samplerateKnob);
    samplerate = samplerate >> 7;
    interfaceClock = 0;
    interfaceClock++;
}

short getKnob() {
  return samplerate;
}

short getbitdepth() {
  return bitdepth;
}

