//This file handles hardware specifics.
#ifndef HARDWARE_H
#define HARDWARE_H
#ifdef DESKTOP
typedef unsigned char bool;
#endif
#define undefined -1
typedef unsigned int word;
char readRAM(word pos);
void writeRAM(word pos, char b);
void writeChar(char c);
char readChar();
bool fileExists(char *fname);
void openFile(char *fname);
void closeFile();
char readFile(word pos);
#ifdef ATMEGA328P
//#include "23LC1024.h"
//#include <SD.h>
//File OPEN_FILE;
#endif
#include "hardware.c"

#endif