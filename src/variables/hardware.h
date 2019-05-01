//This file handles hardware specifics.
#define false 0
#define true 1
#define undefined -1
typedef unsigned int word;
typedef unsigned char bool;
unsigned char RAM[1000];
unsigned char PRG[1000];
char readRAM(word pos);
void writeRAM(word pos, char b);
char readPRG(word pos);
void writePRG(word pos, char b);
char readROM(word pos);
void writeChar(char c);
char readChar();

#include "hardware.c"