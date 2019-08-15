//This file handles hardware specifics.
#ifndef HARDWARE_H
#define HARDWARE_H
#ifdef DESKTOP
typedef unsigned char bool;
#endif
#define undefined -1
#ifdef DESKTOP
typedef unsigned int ibword;
#endif
#ifdef ARDUINO
typedef unsigned short ibword;
#endif
char readRAM(ibword pos);
void writeRAM(ibword pos, char b);
void writeChar(char c);
void waitForInput();
char readChar();
bool fileExistsOnDevice(char *fname);
void openFileOnDevice(char *fname);
void closeFileOnDevice();
char readFileOnDevice(ibword pos);
//Checks if a character is an operator.
bool isOperator(char c);

//Checks if a character is a whitespace character.
bool isWS(char c);

//Checks if a character is an end of line character.
bool isEOL(char c);

#ifdef ATMEGA328P
#include <EEPROM.h>
#include "23LC1024.h"
#include "LCD2004.h"
#include "keyboard.c"
#include "file/file.h"
#endif
#include "hardware.c"

#endif
