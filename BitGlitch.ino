/* Questions
-Switching from UV to ultrabrite cyan LED fixed my interrupt switching issues. wtf?
-Why does ADLAR make my audio quieter
*/

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "analogpins.h"
#include "interface.h"
#include "digitalpins.h"

#define SAMPLE_RATE 12000
#define interfaceSamplerate 256
#define LOOP_LENGTH 1800

//boolean datas. 
bool f_sample=0;
bool readsource = 0; // 0=read audio 1=read buffer
bool storesample = 0; // 0=read audio 1=read buffer
bool stutterFlag = 0; 
bool stutterOn = 0;
bool retrigFlag = 0; 
bool buttflag=0;
bool timer=0;

uint16_t reduced = 0; 
uint16_t sample=0; //sample index

uint8_t sampleIn=0;
volatile uint8_t delaybuffer[LOOP_LENGTH];
uint8_t portread;

ISR(TIMER1_COMPA_vect) {//timer interrupt triggers adc
    if (sample % LOOP_LENGTH == LOOP_LENGTH-1) {
        timer=1; 
    }
    if (readsource==0) {
        sampleIn=adc_read(inputPin);
    } 
    else {
        sampleIn=delaybuffer[(sample)%(LOOP_LENGTH/(getKnob()+1))];
    }
    f_sample=true;
    reduced++;
    sample++;
}

void setup()
{
    cli();//disable interrupts

    setupdigitalIO(); //set deata direction for digital pins
    setupPWM(SAMPLE_RATE);

    //setup analogstuff
    setupanalogIO();
  
    sei();//enableinterrupts
}

ISR(PCINT0_vect) { //button pin change interrupt
  buttflag=1;
}

void loop() //MAIN LOOP
{
    if(f_sample){ //First Check to see if we have a new sample
        if (storesample){
            delaybuffer[sample%LOOP_LENGTH]=sampleIn;
        }
        if(reduced%(getKnob()+1)==0){
        OCR2A = sampleIn;
        f_sample=false;
        }
    } else {
      if(buttflag) {//if PCINT0 was triggered
          portread=PINB;
          stutterFlag = (portread & 1 << 1);//retrieve indv button states
          retrigFlag = (portread & 1 << 0);
          
          if(stutterFlag){
              sample=0;//reset sampleindex
              timer=0;//reset timer
              stutterOn=!stutterOn;//switchstutterstate
              stutterFlag=0;
          }
          if(retrigFlag){
              sample=0;
              timer=0;
              retrigFlag=0;
          }
          buttflag= 0;
      }
    
      //update all interface values
      if (reduced%interfaceSamplerate==0) { 
          updateInterface(); 
      }
  
      //Now that we have the interface info, we need to decide whether we're reading or writing.
      if (!stutterOn) 
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
    }
}


