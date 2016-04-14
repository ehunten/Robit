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
#define regSpeed 650
#define turnSpeed 1000
#define slowSpeed 450

//sensor lower boundaries - it sees tape
//need to be calibrated
#define leftLower 986
#define midLeftLower 987
#define midRightLower 984
#define rightLower 992
//sensor upper boundaries - doesn't see tape
//need to be calibrated
#define leftUpper 992//986 - 1002
#define midLeftUpper 990//987 - 998
#define midRightUpper 990//989 - 999
#define rightUpper 998//992 - 1004



typedef enum stateTypeEnum{
    init, fwd, go, turnRight, turnLeft, objFound, findTape, Tsection, tapeTurnoffLeft, tapeTurnoffRight, done, w1
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
    initSwitch();
    
    
    initIR();
    clearLCD();

    TRISDbits.TRISD2 = 0;
    int i = 0;
    stop();
    
    while (1) {
        
            switch(state) {
                case init:
                    val = midRight;
                    readAdc(val);
                    break;

                case go:
                    printStringLCD("GO");
                    forward();
                    state = fwd;
                    break;
                    
                case fwd:
                    readAdc(val);
                    //clearLCD();
                    motorL = regSpeed;
                    motorR = regSpeed;
                    
                    if (midRight < midRightUpper && midLeft < midLeftUpper && left > leftUpper && right > rightUpper) {
                        state = fwd;
                    }
                    
                    //if there is a left turn
                    if ((left < leftUpper)) {
                   // if ((left < leftUpper || midLeft < midLeftUpper) && midRight > midRightUpper) {
                        state = turnLeft;
                    }
                    
                    
                    //if there is a right turn
                    if (right < rightUpper) {
                    //if ((right < rightUpper || midRight < midRightUpper) && midLeft > midLeftUpper) {
                        state = turnRight;
                    }
                    //if (midRight < midRightUpper && midLeft > midLeftUpper) {
                    //    state = turnRight;
                    //}
                     
                    //LOST THE TAPE
                   // else if (midLeft > midLeftUpper && midRight > midRightUpper && left > leftUpper && right > rightUpper){
                     //   state = findTape;
                        //stop();
                    //}

                    
                    //Intersection
                    if (midLeft < midLeftUpper && midRight < midRightUpper && left < leftUpper && right < rightUpper){
                        state = Tsection;
                    }
                     /*
                    //Tape turnoff
                    if (midLeft < midLeftLower && midRight < midRightLower && left < leftLower) {
                        state = tapeTurnoffLeft;
                    }
                    if (midLeft < midLeftLower && midRight < midRightLower && right < rightLower) {
                        state = tapeTurnoffRight;
                    }
                    */
                    
                    else {
                        forward();
                    }
                    break;
                ////For turns, keep going but one wheel will go faster
                case turnRight:
                    printStringLCD("turnRight");
                    clearLCD();
                    motorL = turnSpeed;
                    motorR = slowSpeed;
                    while (midLeft > midLeftUpper && midRight > midRightUpper);
                    motorR = regSpeed;
                    motorL = regSpeed;
                    state = fwd;
                    break;
                    
                case turnLeft:
                    printStringLCD("turnLeft");
                    clearLCD();
                    motorR = turnSpeed;
                    motorL = slowSpeed;
                    while (midLeft > midLeftUpper && midRight > midRightUpper);
                    motorR = regSpeed;
                    motorL = regSpeed;
                    state = fwd;
                    break;
                ////To be implemented
                case objFound:
                    break;

                ////Lost - spin in place
                ////Would like to find a way to spin 360 deg
                case findTape:
                    printStringLCD("Lost");
                    clearLCD();
                    //motorL = slowSpeed;
                    reverseMotor('R');
                    //motorR = slowSpeed;
                    while (left > leftLower) ;
                    stop();
                    
                        //until the middle sensors find the tape, turn slowly
                        while (midLeft > midLeftUpper && midRight > midRightUpper) {
                            forward();
                            //motorL = regSpeed;
                            //motorR = regSpeed;
                        }
                    if (midLeft < midLeftUpper && midRight < midRightUpper) {
                    reverseMotor('L'); //Set motorR back to forward
                    state = fwd;
                    }
                    break;

                ////Intersection
                case Tsection:
                    printStringLCD("Intersection");
                    clearLCD();
                    
                    if (left > leftUpper && midLeft > midLeftUpper && midRight > midRightUpper && right > rightUpper) {
                        state = done;
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
                    stop();
                    reverseMotor('R');
                    motorR = regSpeed;
                    motorL = regSpeed;
                    if (left < leftUpper) {
                        reverseMotor('L');
                        state = turnRight;
                    }                   
                    break;
                    
                case w1:
                    break;
        }
    }
}


void reverseMotor(char motor) {
    if (motor == 'R') {
    RPF1Rbits.RPF1R = 0b1011;
    RPB10Rbits.RPB10R = 0;
    }
    else if (motor == 'L') {
    RPF1Rbits.RPF1R = 0;
    RPB10Rbits.RPB10R = 0b1011; 
    }

}


//Timer interrupt
void __ISR(_TIMER_3_VECTOR, IPL7SRS) _T3Interrupt () {
    
    PORTD;
    if (PORTDbits.RD6 == 1) {
    IFS0bits.T3IF = 0;
        switch (state) {
            case w1: state = go;
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
            case w1: state = go;
                break;
        }
    }
    
}