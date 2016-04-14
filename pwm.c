/*
 * File:   pwm.c
 * Authors: Evelyn Hunten
 *
 * Created 3/31/16
 */

#include <xc.h>
#include "pwm.h"

#define PRESC1 0 //CHANGE THIS

void initPWM() {

    IFS0bits.T2IF = 0;          // Put the flag down
    TMR2 = 0;                   //Clear TMR2
    PR2 = 1023;                //Set PRvalue
    T2CONbits.TCKPS = PRESC1;   //Set prescalar
    T2CONbits.TCS = 0;          //Set Oscillator
    T2CONbits.ON = 1;          //turn timer on
//THIS IS HOW IT SHOULD BE FOR FORWARD DO NOT TOUCH
    RPF1Rbits.RPF1R = 0;
    RPB10Rbits.RPB10R = 0b1011;
    
    RPD1Rbits.RPD1R = 0b1100;   //map OC1 to RD1 1100
    RPD5Rbits.RPD5R = 0;   //map OC1 to RD5 (ground right now)    
   
//OCM3
    OC3CONbits.ON = 0;      //turn off
    OC3CONbits.OCTSEL = 0;  //using timer 2
    OC3R = 0;
    OC3RS = 0;              //dual compare mode
    OC3CONbits.OCM = 6;     //PWM mode no fault pin
  
//OCM1
    OC1CONbits.ON = 0;
    OC1CONbits.OCTSEL = 0;
    OC1R = 0;
    OC1RS = 0;
    OC1CONbits.OCM = 6;
   
    ///////////////////

    OC1RS = 1000;
    OC1RS = 0;
    OC1CONbits.ON = 1;
    

    OC3RS = 1000;              //dual compare mode
    //OC3RS = 0;
    OC3CONbits.ON = 1;      //turn it on
    
}



void initHbridge() {
    
    
    TRISGbits.TRISG13 = 0;
    TRISDbits.TRISD12 = 0;
    
    LATGbits.LATG13 = 1;
    LATDbits.LATD12 = 1;
    
}

void forward(){
    
    RPF1Rbits.RPF1R = 0;
    RPB10Rbits.RPB10R = 0b1011;
    
    RPD1Rbits.RPD1R = 0b1100;   //map OC1 to RD1 1100
    RPD5Rbits.RPD5R = 0;   //map OC1 to RD5 (ground right now) 
       
}

void backward() {
    RPD1Rbits.RPD1R = 0b1100;   //map OC1 to RD1 1100
    RPD5Rbits.RPD5R = 0;   //map OC1 to RD5 (ground right now)
    
    //RPF1Rbits.RPF1R = 0;
    //RPB10Rbits.RPB10R = 0b1011;
   
}

void stop() {
    
    RPD1Rbits.RPD1R = 0;   //map OC1 to RD1 1100
    RPD5Rbits.RPD5R = 0;   //map OC1 to RD5 (ground right now)
    
    RPF1Rbits.RPF1R = 0;
    RPB10Rbits.RPB10R = 0;
}