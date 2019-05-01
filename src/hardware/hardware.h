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
char readChar();
bool fileExists(char *fname);
void openFile(char *fname);
void closeFile();
char readFile(ibword pos);
#ifdef ATMEGA328P
#include "23LC1024.h"
#endif
#include "hardware.c"

#endif
