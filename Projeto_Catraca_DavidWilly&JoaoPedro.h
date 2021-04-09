#include <16F877A.h>
//#device ADC=8

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O

#use delay(crystal=20000000)


#ifndef lcd_enable
#define lcd_enable pin_e1 // pino enable do LCD
#define lcd_rs pin_e2         // pino rs do LCD
//#define lcd_rw        pin_e2      // pino rw do LCD
#define lcd_d4 pin_d4 // pino de dados d4 do LCD
#define lcd_d5 pin_d5 // pino de dados d5 do LCD
#define lcd_d6 pin_d6 // pino de dados d6 do LCD
#define lcd_d7 pin_d7 // pino de dados d7 do LCD
#endif

#include "library/mod_lcd.c"
#include "library/kbd.c"
#include "library/registro_eeprom.c"

