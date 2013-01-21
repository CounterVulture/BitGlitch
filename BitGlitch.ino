#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "analogpins.h"
#include "interface.h"
#include "digitalpins.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define SAMPLE_RATE 19000
#define speakerPin 11
#define LOOP_LENGTH 1800

//boolean datas. 
bool f_sample=false;
bool readsource = 0; // 0=read audio 1=read buffer
bool storesample = 0; // 0=read audio 1=read buffer
bool stutterStatus = 0; 
bool stutterOn = 0;
bool retrigStatus = 0; 
bool buttflag=0;
bool timer=0;
//ints. should be counters or else the datasize may be wasteful
//use uints when unsigned

int bufferlength=LOOP_LENGTH;
int reduced = 0; 
int sample=0; //sample index

byte sampleIn=0;
volatile byte delaybuffer[LOOP_LENGTH];
byte portread;

ISR(TIMER1_COMPA_vect) {//timer interrupt triggers adc
  if (sample % bufferlength == bufferlength-1) {
    timer=1; 
  }
  if (readsource==0) {
    sampleIn=adc_read(inputPin);
  } 
  else {
    sampleIn=delaybuffer[(sample)%(bufferlength/(getKnob()+1))];
  }
  f_sample=true;
  reduced++;
  sample++;
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
  cli();//disable interrupts

  setupdigitalIO(); //set deata direction for digital pins
  //PCMSK1 |= (1<<PCINT8);????
  //PCMSK1 |= (1<<PCINT9);

  //setup analogstuff
  setupanalogIO();

  //ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADEN); //enable ADC
  //ADCSRA |= (1<<ADSC);//Do an initial conversion because 
  //this one is the slowest and to ensure that everything is up and running
  setupPWM();//??
  sei();//enableinterrupts
}

ISR(PCINT0_vect) { //button pin change interrupt
  buttflag=1;
}

void loop()
{
  if(buttflag== 1) //if PCINT0 was triggered
  {
    portread=PINB&PCMSK0;//read pin port masked by intmask (necesary?)
    stutterStatus = (portread & 1 << 1);//retrieve indv button states
    retrigStatus = (portread & 1 << 0);
    
    //check if the buttons were just switched on before setting the new states.
    if(stutterStatus==1&&stutterStatus!=getbuttonState()){
      sample=0;//reset sampleindex
      timer=0;//reset timer
      stutterOn=!stutterOn;//switchstutterstate
    }
    if(retrigStatus==1&&retrigStatus!=getretrigState()){
      sample=0;
      timer=0;
    }
    buttflag= 0;
  } //end button interrupt routine
  
  //update all interface values
  if (reduced%interfaceSamplerate==0) { 
    updateInterface(); 
    //set new buttonstates
    setbuttonState(stutterStatus); 
    setretrigState(retrigStatus);
  }

  //Now that we have the interface info, we need to decide whether we're
  //reading or writing right now
  if (stutterOn==0) 
  {
    readsource=0;
    storesample=0;
  } 
  else if (timer==0) {
    readsource=0; 
    storesample=1;
  } 
  else {
    readsource=1;
    storesample=0;
  }

  if(f_sample){ //when the next sample has been read
    if (storesample==1){
      delaybuffer[sample%bufferlength]=sampleIn;
    }
    if(reduced%(getKnob()+1)==0){
    OCR2A = sampleIn;
    f_sample=false;
    }
  }
}

void setupPWM()
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
  // OCR1A is a 16-bit register, so we have to do this with
  // interrupts disabled to be safe.
  OCR1A = F_CPU / SAMPLE_RATE;    // 16e6 / 8000 = 2000

  TIMSK1 |= _BV(OCIE1A);  // Enable interrupt when TCNT1 == OCR1A
}
