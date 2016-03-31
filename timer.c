/*
* File:   timer.c
* Author: Evelyn Hunten
*
* Created 3/31/16
*/

#include <xc.h>
#include "timer.h"
#include "lcd.h"

#define PRESC1 0
#define PRESC256 1
#define CLEAR 0
#define ENABLE 1
#define DEFAULT 7
#define PRVAL 1249


void initTimer1(){
    //Initialize timer 1
    IFS0bits.T1IF = CLEAR;          // Put the flag down
    TMR1 = CLEAR;                   // clear TMR1
    PR1 = 9;                      // Initialize PR1 - should be 10milliseconds
    T1CONbits.TCKPS = PRESC1;     // Initialize pre-scalar
    T1CONbits.TCS = CLEAR;          // Set the oscillator
   // IEC0bits.T1IE = ENABLE;         // Enable the interrupt
   // IPC1bits.T1IP = DEFAULT;        // Configure the Interrupt Priority
    T1CONbits.ON = ENABLE;          // Turn the timer on
}

void initTimer3(){
    IFS0bits.T3IF = 0;// Put the flag down
    TMR3 = 0;// clear TMR1
    PR3 = 39061;// Initialize PR1
    T3CONbits.TCKPS = 3; // Initialize pre-scalar
    T3CONbits.TCS = 0; // Setting the oscillator
    IEC0bits.T3IE = 1;// Enable the interrupt
    IPC3bits.IC3IP = 7;// Configure the Interrupt Priority
    T3CONbits.ON = 1;// Turn the timer on
}

void stopTimer() {
        T3CONbits.ON = 0;// Turn the timer off
        IEC0bits.T3IE = 0;// disable the interrupt
}


//Uses timer 1
void delayUs(unsigned int delay){
    //Create a delay using timer 2 for "delay" microseconds.      
     TMR1 = 0;
     PR1 = delay*PRVAL;
     IFS0bits.T1IF = 0;
     T1CONbits.ON = 1;
     
      while (IFS0bits.T1IF == CLEAR);
      T1CONbits.ON = CLEAR;
      IFS0bits.T1IF = 0;
     
}
