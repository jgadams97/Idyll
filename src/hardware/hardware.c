//This file handles hardware specifics.

#ifdef DESKTOP

#include <stdio.h>
#define false 0
#define true 1
unsigned char RAM[125000];
FILE *OPEN_FILE;

//Get size of RAM.
ibword sizeRAM() {
	return 125000;
}

//Read a byte from RAM.
char readRAM(ibword pos) {
	return RAM[pos];
}

//Write a byte to RAM. 
void writeRAM(ibword pos, char b) {
	RAM[pos] = b;
}

//Write a character to the screen.
void writeChar(char c) {
	printf("%c", c);
}

//Read a character from the keyboard.
char readChar() {
	char c;
	scanf("%c", &c);
	return c;
}


void backspace() {};

//Check if a file exists.
bool fileExistsOnDevice(char *fname) {
	FILE *file = fopen(fname, "r");
	if (file == NULL) {
		return false;
	} else {
		fclose(file);
		return true;
	}
}

//Open a file.
void openFileOnDevice(char *fname) {
	OPEN_FILE = fopen(fname, "r");
}

//Closes a file.
void closeFileOnDevice() {
	fclose(OPEN_FILE);
}

//Read character from file at address.
char readFileOnDevice(ibword pos) {
	fseek(OPEN_FILE, pos, SEEK_SET);
	return (char)fgetc(OPEN_FILE);
}

ibword sizeOfFileOnDevice() {
	fseek(OPEN_FILE, 0L, SEEK_END);
	return ftell(OPEN_FILE);
}


#endif

#ifdef ATMEGA328P

void writeChar(char c) {
	lcdPutChar(c);
}

char readChar() {
	char c = kbGetChar();
	if (c != (char)-1 && c != 0 && c != 0x08) lcdPutChar(c);
	return c;
}

void backspace() {
	lcdBackspace();
	lcdPutChar(' ');
	lcdBackspace();
}

ibword sizeRAM() {
	return 64000;
}

//File RAM_FILE;
//Read a byte from RAM.
char readRAM(ibword pos) {
	return sramRead(pos);
}

//Write a byte to RAM. 
void writeRAM(ibword pos, char b) {
	sramWrite(pos, b);
}

File OPEN_FILE;

//Check if a file exists.
bool fileExistsOnDevice(char *fname) {
	OPEN_FILE = openFile(NULL, fname);
	if (OPEN_FILE.status == 0) return 1;
	return 0;
}

//Open a file.
void openFileOnDevice(char *fname) {
	OPEN_FILE = openFile(NULL, fname);
}

//Closes a file.
void closeFileOnDevice() {}

//Read character from file at address.
ibword FILE_PPOS = -1;
char readFileOnDevice(ibword pos) {
	if (FILE_PPOS != pos - 1 && pos != 0)
		seekFile(&OPEN_FILE, pos);
	FILE_PPOS = pos;
	return readFile(&OPEN_FILE);
}

ibword sizeOfFileOnDevice() {
	return OPEN_FILE.size;
}

#endif
