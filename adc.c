/*
 * File:   adc.c
 * Authors: Evelyn Hunten
 *
 * Created: 3/31/16
 */

/* TODO: Make define statements for each pin used in the LCD
 */
#include <xc.h>
#include "adc.h"

void initADC(){
    
    ANSELBbits.ANSB0 = 0; 
    
    AD1CON1bits.ADON = 0; //turn off the ADC
    
    AD1CON1bits.FORM = 0; // 16 unsigned integer
    AD1CON1bits.SSRC = 7; // Auto-convert mode
    AD1CON1bits.ASAM = 1; // Auto-sampling
    AD1CON2bits.VCFG = 0; // Use board reference voltages
    AD1CON2bits.CSCNA = 0; // Disable scanning
    AD1CON2bits.SMPI = 15; // 15 burritos
    AD1CON2bits.ALTS = 0; // Only Mux A
    AD1CON3bits.ADRC = 0; // Use PBCLK
    AD1CON3bits.SAMC = 2; // 2 Tad per sample
    AD1CON3bits.ADCS = 0xFF; // 512 times the PBCLK
    AD1CHSbits.CH0NA = 0; // Use Vref- as negative reference
    AD1CHSbits.CH0SA = 0; // Scan AN0 at least
    IFS0bits.AD1IF = 0; // Put down ADC flag
    
    AD1CON1bits.ADON = 1; // turn on the ADC

}

