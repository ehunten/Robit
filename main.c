// ******************************************************************************************* //
//
// File:         main.c
// Date:         3/31/16
// Authors:      Evelyn Hunten
//
// Description: Mr. Roboto
// ******************************************************************************************* //

#include <xc.h>
#include <sys/attribs.h>
#include "adc.h"
#include "config.h"
#include "interrupt.h"
#include "pwm.h"
#include "timer.h"
#include "switch.h"
#include "irLED.h"
#include "lcd.h"

#define left ADC1BUF0
#define midLeft ADC1BUF1
#define midRight ADC1BUF2
#define right ADC1BUF3
#define motorL OC3RS
#define motorR OC1RS
#define regSpeed 700
#define turnSpeed 600
#define slowSpeed 500

//sensor lower boundaries - it sees tape
//need to be calibrated
#define leftLower 200
#define midLeftLower 80
#define midRightLower 115
#define rightLower 115
//sensor upper boundaries - doesn't see tape
//need to be calibrated
#define leftUpper 700
#define midLeftUpper 340
#define midRightUpper 140
#define rightUpper 140



typedef enum stateTypeEnum{
    init, fwd, turnRight, turnLeft, objFound, findTape, Tsection, tapeTurnoffLeft, tapeTurnoffRight, done, w1
} stateType;

volatile stateType state = init;

volatile float val = 0;
volatile float Input = 0;
volatile float doneLine = 0;


int main(void)
{
    SYSTEMConfigPerformance(40000000);
    enableInterrupts();
    initTimer1();
    initADC();
    initLCD();
    initHbridge();
    initPWM();
    
    initIR();
    clearLCD();
    fwd();

    TRISDbits.TRISD2 = 0;
    int i = 0;
    
    
    while (1) {
        
            switch(state) {
                case init:
                    //if (left > 100 && midLeft > 100 && midRight > 100 && right > 100){
                        //readySetGo();
                       // LATDbits.LATD2 = 1;
                    //}
                    val = midLeft;
                    readAdc(val);
                    //state = fwd;
                    break;

                case fwd:
                    printStringLCD("Fwd");
                    clearLCD();
                    motorL = regSpeed;
                    motorR = regSpeed;
                    //if there is a left turn
                    if (left < leftLower && right > rightUpper) {
                        state = turnLeft;
                    }
                    //if there is a right turn
                    if (right < rightLower && left > leftUpper) {
                        state = turnRight;
                    }
                    //LOST THE TAPE
                    if (midLeft > midLeftUpper && midRight > midRightUpper && left > leftUpper && right > rightUpper){
                        state = findTape;
                    }
                    //Intersection
                    if (midLeft > midLeftLower && midRight > midRightLower && left > leftLower && right > rightLower){
                        state = Tsection;
                    }

                    //Tape turnoff
                    if (midLeft < midLeftLower && midRight < midRightLower && left < leftLower) {
                        state = tapeTurnoffLeft;
                    }
                    if (midLeft < midLeftLower && midRight < midRightLower && right < rightLower) {
                        state = tapeTurnoffRight;
                    }
                    
                    break;
                ////For turns, keep going but one wheel will go faster
                case turnRight:
                    printStringLCD("turnRight");
                    clearLCD();
                    while (left < leftUpper) {
                        motorL = turnSpeed;
                    }
                    state = fwd;
                    break;
                    
                case turnLeft:
                    printStringLCD("turnLeft");
                    clearLCD();
                    while (right < rightLower) {
                        motorR = turnSpeed;
                    }
                    state = fwd;
                    break;
                ////To be implimented
                case objFound:
                    break;

                ////Lost - spin in place
                ////Would like to find a way to spin 360 deg
                case findTape:
                    printStringLCD("Lost");
                    clearLCD();
                    motorL = turnSpeed;
                    reverseMotor('R');
                    motorR = turnSpeed;
                    while (left > leftLower) ;
                    
                    if (left < leftLower) {
                        //until the middle sensors find the tape, turn slowly
                        while (midLeft > midLeftUpper && midRight > midRightUpper) {
                            motorL = slowSpeed;
                            motorR = slowSpeed;
                        }
                    reverseMotor('F'); //Set motorR back to forward
                        state = fwd;
                    }
                    break;

                ////Intersection
                case Tsection:
                    printStringLCD("Intersection");
                    clearLCD();
                //wait to count line until after wheel've passed it
                    while (left < leftLower && right < rightLower) ;
                    doneLine ++;
                    if (doneLine == 3) {
                        state = done;
                    }
                    else {
                        state = fwd;
                    }
                    break;

                ////For now we are not trying to do the D loop
                case tapeTurnoffLeft:
                    printStringLCD("SharpLeft");
                    clearLCD();
                    state = fwd;
                    break;

                case tapeTurnoffRight:
                    printStringLCD("SharpRight");
                    clearLCD();
                    state = fwd;
                    break;
                    
                case done:
                    printStringLCD("TURNAROUND");
                    clearLCD();
                //turn around and do the course again
                    state = findTape;                    
                    break;
                    
                case w1:
                    break;
        }
    }
}


void reverseMotor(char motor) {
    if (motor == 'R') {
        RPD1Rbits.RPD1R = 0;
        RPD5Rbits.RPD5R = 0b1100;
    }
    else if (motor == 'L') {
        RPD1Rbits.RPD1R = 0b1100;
        RPD5Rbits.RPD5R = 0; 
    }

}


//Timer interrupt
void __ISR(_TIMER_3_VECTOR, IPL7SRS) _T3Interrupt () {
    
    PORTD;
    if (PORTDbits.RD6 == 1) {
    IFS0bits.T3IF = 0;
        switch (state) {
            case w1: state = fwd;
                break;
        }
    }

}

//Button change notification
void __ISR(_CHANGE_NOTICE_VECTOR, IPL7SRS) _CNInterrupt () {
    
    IFS1bits.CNDIF = 0;                 // Put down the flag
    PORTD;
    
    LATDbits.LATD0 = 0;
    
    if (PORTDbits.RD6 == 0) {
        initTimer3();
        switch (state) {
            case init: state = w1;
            break;
        }
            
    }
    else if (PORTDbits.RD6 == 1) {
        IEC0bits.T3IE = 0;
        T3CONbits.ON = 0;
        switch (state) {
            case w1: state = fwd;
                break;
        }
    }
    
}