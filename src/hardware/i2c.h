#ifndef LIB_I2C
#define LIB_I2C

//Pins.
char I2C_PIN_SCL;
char I2C_PIN_SDA;

//Read a digital pin (open collector).
char i2cDigitalRead(char pin);

//Write a digital pin (open collector).
void i2cDigitalWrite(char pin, char val);

//Setup I2C.
void i2cSetup(char pinSDA, char pinSCL);

//Begin a transmission.
void i2cBeginTransmission();

//End a transmission.
void i2cEndTransmission();

//Send a byte.
void i2cSend(char b);

//Read a byte.
char i2cReceive();

//Check for acknowledge.
char i2cAcknowledge();

#include "i2c.c"

#endif
