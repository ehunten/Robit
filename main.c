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
#define regSpeed 600
#define turnSpeed 800
#define slowSpeed 500

//sensor lower boundaries - it sees tape
//need to be calibrated
#define leftLower 115
#define midLeftLower 115
#define midRightLower 115
#define rightLower 115
//sensor upper boundaries - doesn't see tape
//need to be calibrated
#define leftUpper 140
#define midLeftUpper 140
#define midRightUpper 140
#define rightUpper 140



typedef enum stateTypeEnum{
    init, fwd, turnRight, turnLeft, objFound, findTape, Tsection, done
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

    TRISDbits.TRISD2 = 0;
    int i = 0;
    
    
    while (1) {
        
            switch(state) {
                case init:
                    if (left > 100 && midLeft > 100 && midRight > 100 && right > 100){
                        readySetGo();
                        LATDbits.LATD2 = 1;
                    }
                    val = midLeft;
                    readAdc(val);
                    //state = fwd;
                    break;

                case fwd:
                    motorL = regSpeed;
                    motorR = regSpeed;
                    //if there is a right turn
                    if (left < leftLower && right > rightUpper) {
                        state = turnRight;
                    }
                    //if there is a left turn
                    if (right < rightLower && left > leftUpper) {
                        state = turnLeft;
                    }
                    //LOST THE TAPE
                    if (midLeft > midLeftUpper && midRight > midRightUpper && left > leftUpper && right > rightUpper){
                        state = findTape;
                    }
                    //Intersection
                    if (midLeft > midLeftLower && midRight > midRightLower && left > leftLower && right > rightLower){
                        state = Tsection;
                    }
                    
                    break;
                ////For turns, keep going but one wheel will go faster
                case turnRight:
                    while (left < leftUpper) {
                        motorL = turnSpeed;
                    }
                    state = fwd;
                    break;
                    
                case turnLeft:
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
                    
                case done:
                //turn around and do the course again
                    state = findTape;                    
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