/*
 * File:   irLED.c
 * Authors: Evelyn Hunten
 *
 * Created: 3/31/16
 */

#include <xc.h>
#include "irLED.h"

#define TRISLED1 TRISDbits.TRISD11
#define TRISLED2 TRISCbits.TRISC14
#define TRISLED3 TRISBbits.TRISB5
#define TRISLED4 TRISBbits.TRISB12
#define LED1 LATDbits.LATD11
#define LED2 LATCbits.LATC14
#define LED3 LATBbits.LATB5
#define LED4 LATBbits.LATB12

#define OUT 0

void initIR() {
    TRISLED1 = OUT;
    TRISLED2 = OUT;
    TRISLED3 = OUT;
    TRISLED4 = OUT;
    
    LED1 = 1;
    LED2 = 1;
    LED3 = 1;
    LED4 = 1;
    
}
