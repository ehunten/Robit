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
#define ON 1

void initIR() {
    TRISLED1 = OUT;
    TRISLED2 = OUT;
    TRISLED3 = OUT;
    TRISLED4 = OUT;
    
    LED1 = ON;
    LED2 = ON;
    LED3 = ON;
    LED4 = ON;
    
}

readySetGo() {
    //changed this function - now all it does is delay so we can get better readings off the LCD for testing
    int i = 0;
    for (i = 0; i < 200; i++) 
    {
        delayUs(100);
    }
}
