//This file handles hardware specifics.
#ifndef HARDWARE_H
#define HARDWARE_H
#ifdef DESKTOP
typedef unsigned char bool;
#endif
#define undefined -1
#ifdef DESKTOP
typedef int ibword;
#endif
#ifdef ARDUINO
typedef long ibword;
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
#ifdef ATMEGA328P
#include "23LC1024.h"
#include "LCD2004.h"
#include "file/file.h"
#endif
#include "hardware.c"

#endif
