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

#define BUF0 ADC1BUF0
#define BUF1 ADC1BUF1
#define BUF2 ADC1BUF2
#define BUF3 ADC1BUF3

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
    
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    int i = 0;
    
    
    while (1) {
        
            switch(state) {
                case init:
                    //if (BUF1 < 130 && BUF2 < 130 && BUF0 > 145 && BUF3 > 145) {
                    if (BUF1 > 100 && BUF2 > 100 && BUF0 > 100 && BUF3 > 100){
                        readySetGo();
                    }
                    val = BUF1;
                    readAdc(val);
                    state = fwd;
                    break;

                case fwd:
                    OC3RS = 300;
                    OC1RS = 300;
                    if (BUF0 < 130 && BUF3 > 140) {
                        state = turnRight;
                    }
                    if (BUF3 < 130 && BUF0 > 140) {
                        state = turnLeft;
                    }
                    //LOST THE TAPE
                    if (BUF1 > 145 && BUF2 > 145 && BUF0 > 145 && BUF3 > 145){
                        state = findTape;
                    }
                    
                    //FOUND AN INTERSECTION
                    if (BUF1 > 130 && BUF2 > 130 && BUF0 > 130 && BUF3 > 130){
                        state = Tsection;
                    }
                    
                    break;

                case turnRight:
                    while (BUF0 < 140) {
                        OC3RS = 400;
                    }
                    state = fwd;
                    break;
                    
                case turnLeft:
                    while (BUF3 < 140) {
                        OC1RS = 400;
                    }
                    state = fwd;
                    break;

                case objFound:
                    break;

                case findTape:
                    OC3RS = 300;
                    OC1RS = 0;
                    while (BUF0 > 130) ;
                    
                    if (BUF0 < 130) {
                        while (BUF1 > 140 && BUF2 > 140) {
                            OC3RS = 300;
                        }
                        state = fwd;
                    }
                    break;

                case Tsection:
                    for (i = 0; i < 50000; i++) {
                        
                    }
                    doneLine ++;
                    if (doneLine == 3) {
                        state = done;
                    }
                    else {
                        state = fwd;
                    }
                    break;
                    
                case done:
                    state = findTape;                    
                    break;
        }
    }
}