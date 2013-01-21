#ifndef INTERFACE_H
#define INTERFACE_H
  //IMPORT THIS INTO ANALOGPINS
  #define ledPin 13
  #define samplerateKnob 4
  #define bitdepthKnob 5
  #define stutterButton 9
  #define retrigButton 8
  #define interfaceSamplerate 512
  
  void updateInterface();
  short getKnob();
  short getbitdepth();
  bool getbuttonState();
  bool getretrigState();
  void setbuttonState(bool state);
  void setretrigState(bool state);
  
#endif
