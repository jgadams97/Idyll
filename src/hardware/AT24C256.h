#ifndef LIB_AT24C256
#define LIB_AT24C256

#include "i2c.h"
char EEPROM_ADDRESS;
char EEPROM_STATUS;

//Write to EEPROM.
void eepromWrite(long addr, char val);

//Read from EEPROM.
char eepromRead(long addr);

//Get status of recent read/write.
char eepromSuccess();

//Setup EEPROM.
char eepromSetup(char addr, int pinSDA, int pinSCL);

#include "AT24C256.c"

#endif
