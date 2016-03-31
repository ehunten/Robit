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


typedef enum stateTypeEnum{
    s1, s2, s3, s4, w1, w2, w3, w4
} stateType;

volatile stateType state = s1;

volatile float val = 0;
volatile float Input = 0;


int main(void)
{
    SYSTEMConfigPerformance(40000000);
    enableInterrupts();
    initTimer1();
    initADC();
    initHbridge();
    initPWM();
    initSwitch();

    
    while (1) {
        
    }
}