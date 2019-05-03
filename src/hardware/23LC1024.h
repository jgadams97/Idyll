#ifndef LIB_23LC1024
#define LIB_23LC1024

//Pin definitions.
char SRAM_PIN_CS;
char SRAM_PIN_SCK;
char SRAM_PIN_SO;
char SRAM_PIN_SI;

//Shift out a byte.
void SRAM_CLOCK_BYTE_OUT(char b);

//Shift in a byte.
char SRAM_CLOCK_BYTE_IN();

//Set read mode.
char SRAM_READ_MODE();

//Set write mode.
void SRAM_WRITE_MODE(char m);

//Initialize SRAM chip in Byte Write mode.
void sramSetup(char pinCS, char pinSCK, char pinSO, char pinSI);

//Writes byte b to address a.
void sramWrite(long a,  char b);

//Reads byte from address a.
char sramRead(long a);

#include "23LC1024.c"

#endif
