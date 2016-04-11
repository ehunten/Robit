/*
 * File:   lcd.c
 * Authors: Evelyn Hunten
 *
 * Created: 2/16/16
 */

/* TODO: Make define statements for each pin used in the LCD
 */
#include <xc.h>
#include "lcd.h"
#include "timer.h"

/* The pins that you will use for the lcd control will be
 * LCD_RS      RC4
 * LCD_E       RC2
 * LCD_D4      RE7
 * LCD_D5      RE5
 * LCD_D6      RE3
 * LCD_D7      RE1
 * Use these so that we will be able to test your code for grading and to
 * help you debug your implementation!
 */

#define LCD_DATA  
#define LCD_RS  LATCbits.LATC4
#define LCD_E   LATCbits.LATC2

#define TRIS_D7 TRISEbits.TRISE1
#define TRIS_D6 TRISEbits.TRISE3
#define TRIS_D5 TRISEbits.TRISE5 
#define TRIS_D4 TRISEbits.TRISE7
#define TRIS_RS TRISCbits.TRISC4
#define TRIS_E  TRISCbits.TRISC2

#define LAT_D7 LATEbits.LATE1
#define LAT_D6 LATEbits.LATE3
#define LAT_D5 LATEbits.LATE5
#define LAT_D4 LATEbits.LATE7

/* This function should take in a two-byte word and writes either the lower or upper
 * byte to the last four bits of LATE. Additionally, according to the LCD data sheet
 * It should set LCD_RS and LCD_E with the appropriate values and delays.
 * After these commands are issued, there should be a delay.
 * The command type is a simplification. From the data sheet, the RS is '1'
 * when you are simply writing a character. Otherwise, RS is '0'.
 */
void writeFourBits(unsigned char word, unsigned int commandType, unsigned int delayAfter, unsigned int lower){
    //If lower is 0, write upper
    // set the commandType (RS value)
    if (commandType == 0)
    {
        LCD_RS = 0;
    }
    else if (commandType == 1)
    {
        LCD_RS = 1;
    }
    
    switch(lower) {
        case 1:              //Lower
            LAT_D4 = word&0x01;
            LAT_D5 = (word >> 1)&0x01;
            LAT_D6 = (word >> 2)&0x01;
            LAT_D7 = (word >> 3)&0x01;
            break;
    
        case 0:              //Upper
            LAT_D4 = (word >> 4)&0x01;
            LAT_D5 = (word >> 5)&0x01;
            LAT_D6 = (word >> 6)&0x01;
            LAT_D7 = (word >> 7)&0x01;
            break;
    }
    
    //enable
    LCD_E = 1;
    
    //delay
    //delayUs(500); 
    
    //disable
    LCD_E = 0;
    
    //delayUs(500);
    delayUs(delayAfter);
}

/* Using writeFourBits, this function should write the two bytes of a character
 * to the LCD.
 */
void writeLCD(unsigned char word, unsigned int commandType, unsigned int delayAfter){
        
    writeFourBits(word, commandType, delayAfter, 0);
    writeFourBits(word, commandType, delayAfter, 1);    
}

/* Given a character, write it to the LCD. RS should be set to the appropriate value.
 */
void printCharLCD(char c) {
    writeLCD(c, 1, 40);
}



/*Initialize the LCD
 */
void initLCD(void) {
    // Setup D, RS, and E to be outputs (0).
    TRIS_E = 0; //E
    TRIS_RS = 0; //RS
    TRIS_D4 = 0; //D4
    TRIS_D5 = 0; //D5
    TRIS_D6 = 0; //D6
    TRIS_D7 = 0; //D7
    // Initilization sequence utilizes specific LCD commands before the general configuration
    // commands can be utilized. The first few initilization commands cannot be done using the
    // WriteLCD function. Additionally, the specific sequence and timing is very important.
    LCD_E = 0;
    
    delayUs(20000); //delay 20ms

    // Enable 4-bit interface
    writeFourBits(0b0011, 0, 4200, 1);
    writeFourBits(0b0011, 0, 500, 1);
    
    writeFourBits(0b0011, 0, 500, 1);
    writeFourBits(0b0010, 0, 500, 1);
    
        // Function Set (specifies data width, lines, and font.
    writeLCD(0b00101000, 0, 500);
    
    writeLCD(0b00001000, 0, 500);
    
    writeLCD(0b00000001, 0, 1640);
    writeLCD(0b00000110, 0, 500);
    
    writeLCD(0b00001110, 0, 500);
}

/*
 * You can use printCharLCD here. Note that every time you write a character
 * the cursor increments its position automatically.
 * Since a string is just a character array, try to be clever with your use of pointers.
 */
void printStringLCD(const char* s) {
    int i = 0;
    for (i = 0; s[i] != '\0'; i++) {
        writeLCD(s[i], 1, 1);
    }
    
}

/*
 * Clear the display.
 */
void clearLCD(){
   
    writeLCD(0x01, 0, 40);
    delayUs(1640); //delay is specified in init function
}

/*
 Use the command for changing the DD RAM address to put the cursor somewhere.
 */
void moveCursorLCD(int top){
    if (top == 0) {
    writeLCD(0b11000000, 0, 40);
    }
    else if (top == 1) {
    writeLCD(0b10000000, 0, 40);
    }
}

/*
 * This function is called in lab1p2.c for testing purposes.
 * If everything is working properly, you should get this to look like the video on D2L
 * However, it is suggested that you test more than just this one function.
 */
void testLCD(){
    initLCD();
    int i = 0;
    printCharLCD('c');
    for(i = 0; i < 1000; i++) delayUs(1000);
    clearLCD();
    printStringLCD("Hello!");
    //moveCursorLCD(1, 2);
    for(i = 0; i < 1000; i++) delayUs(1000);
    printStringLCD("Hello!");
    for(i = 0; i < 1000; i++) delayUs(1000);
}

