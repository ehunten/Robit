#include <xc.h>
#include "switch.h"


void initSwitch () {
    //RD6
    TRISDbits.TRISD6 = 1;           // Configure switch as input
    CNCONDbits.ON = 1;                  // Enable overall interrupt
    CNENDbits.CNIED6 = 1;         // Enable pin CN
    CNPUDbits.CNPUD6 = 1;         // Enable pull-up resistor
    IFS1bits.CNDIF = 0;                 // Put down the flag
    IPC8bits.CNIP = 7;                  // Configure interrupt priority
    IEC1bits.CNDIE = 1;           // Enable interrupt for D pins
    
}