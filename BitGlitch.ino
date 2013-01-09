#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "input.h"
#include "interface.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define SAMPLE_RATE 10000
#define speakerPin 11
#define LOOP_LENGTH 1800

//boolean datas. 
bool f_sample=false;
bool readsource = 0; // 0=read audio 1=read buffer
bool storesample = 0; // 0=read audio 1=read buffer
bool stutterStatus = 0; 
bool stutterOn = 0;
bool retrigStatus = 0; 

//ints. should be counters or else the datasize may be wasteful
//use uints when unsigned
int bufferlength=LOOP_LENGTH;
int reduced = 0; 
int timer=0;
int sample=0; //sample index

byte sampleIn=0;
volatile byte delaybuffer[LOOP_LENGTH];
byte portread;

ISR(TIMER1_COMPA_vect) {

    if (sample % bufferlength == bufferlength-1) {
      timer++; 
    }
    if (readsource==0) {
      sampleIn=adc_read(inputPin);
    } else {
      sampleIn=delaybuffer[(sample)%(bufferlength/(getKnob()+1))];
    }
  f_sample=true;
  reduced++;
  sample++;
}

void startPlayback()
{
  pinMode(speakerPin, OUTPUT);

  // Set up Timer 2 to do pulse width modulation on the speaker
  // pin.

  // Use internal clock (datasheet p.160)
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));

  // Set fast PWM mode  (p.157)
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B &= ~_BV(WGM22);

  // Do non-inverting PWM on pin OC2A (p.155)
  // On the Arduino this is pin 11.
  TCCR2A = (TCCR2A | _BV(COM2A1)) & ~_BV(COM2A0);
  TCCR2A &= ~(_BV(COM2B1) | _BV(COM2B0));

  // No prescaler (p.158)
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

  // Set up Timer 1 to send a sample every interrupt.

  cli();

  // Set CTC mode (Clear Timer on Compare Match) (p.133)
  // Have to set OCR1A *after*, otherwise it gets reset to 0!
  TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
  TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));

  // No prescaler (p.134)
  TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

  // Set the compare register (OCR1A).
  // OCR1A is a 16-bit register, so we have to do this with
  // interrupts disabled to be safe.
  OCR1A = F_CPU / SAMPLE_RATE;    // 16e6 / 8000 = 2000

  // Enable interrupt when TCNT1 == OCR1A (p.136)
  TIMSK1 |= _BV(OCIE1A);

  sei();
}

void stopPlayback()
{
  // Disable playback per-sample interrupt.
  TIMSK1 &= ~_BV(OCIE1A);

  // Disable the per-sample timer completely.
  TCCR1B &= ~_BV(CS10);

  // Disable the PWM timer.
  TCCR2B &= ~_BV(CS10);
}

void setup()
{
  setADCPrescaler();
  startPlayback();
  
  //set up analog/digital port direction
  DDRD = DDRD | B1111100; //set Digital 2-7 to Out
  DDRB = B00101000; //set Digital Pin 11 to out, 13 to out, rest to in
  //this also sets a value of 0 for ports 14 and 15 which are crystals ports
  //does this have an effect?
  DDRC = B00000000; //set analog to input
}

void loop()
{
  //update all interface values and do some math
  if (reduced%interfaceSamplerate==0) { 
    updateInterface(); 
    //obtain button states from port B (this should eventually be in interfaceupdate
    portread=PINB;
    stutterStatus = (portread & 1 << 1);
    retrigStatus = (portread & 1 << 0);
    
    //check if the buttons were just switched on before setting the new states.
    if(stutterStatus==1&&stutterStatus!=getbuttonState()){
      sample=0;
      timer=0;
      //stutterOn=~stutterOn;
      if(stutterOn==0){stutterOn=1;}
      else {stutterOn=0;}
    }
    
    if(retrigStatus==1&&retrigStatus!=getretrigState()){
      sample=0;
      timer=0;
    }
    //set new states
    setbuttonState(stutterStatus);
    setretrigState(retrigStatus);
  }
  
  //Now that we have the button info, we need to decide whether we're
  //reading or writing right now
  if (stutterOn==0) 
  {
    readsource=0;
    storesample=0;
    cbi(PORTB,5);//ledON
  } else if (timer==0) {
    readsource=0; 
    storesample=1;
    sbi(PORTB,5);//ledOFF
  } else if (timer>0&&timer<4){
    readsource=1;
    storesample=0;
    sbi(PORTB,5);//ledOFF
  }
  
  if(f_sample==true){ //when the next sample has been read
    if (storesample==1){
      delaybuffer[sample%bufferlength]=sampleIn;
    }
    //if(reduced%(getKnob()+1)==0){
      OCR2A = sampleIn;
      f_sample=false;
    //}
  }
}
