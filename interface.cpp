#include "interface.h"
#include "Arduino.h"

//IMPORT THIS INTO ANALOGPINS
int interfaceClock;
short samplerate;
short bitdepth;

bool buttonState = 0;
bool retrigState = 0;

void updateInterface() {
    samplerate = analogRead(samplerateKnob);
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
bool getbuttonState() {
  return buttonState;
}

bool getretrigState() {
 return retrigState;
}

void setbuttonState(bool state) {
  buttonState=state;
  return;
}

void setretrigState(bool state) {
  retrigState=state;
  return;
}
