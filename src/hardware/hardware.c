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

//Check if a file exists.
bool fileExists(char *fname) {
	FILE *file = fopen(fname, "r");
	if (file == NULL) {
		return false;
	} else {
		fclose(file);
		return true;
	}
}

//Open a file.
void openFile(char *fname) {
	OPEN_FILE = fopen(fname, "r");
}

//Closes a file.
void closeFile() {
	fclose(OPEN_FILE);
}

//Read character from file at address.
char readFile(ibword pos) {
	fseek(OPEN_FILE, pos, SEEK_SET);
	return (char)fgetc(OPEN_FILE);
}

#endif

#ifdef ATMEGA328P

void writeChar(char c) {
	Serial.print(c);
}

char readChar() {
	return Serial.read();
}

ibword sizeRAM() {
	return 125000;
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


//const char myFile[] = "clear\ndim tmp = 0\ndim pfib = 1\ndim fib = 1\ndim $out[100] = \"1, 1\"\n@loop\n	tmp = fib\n	fib = pfib + fib\n	pfib = tmp\n	$out = $out + \", \" + fib\n\n	\nif pfib <= 1000 then\n	goto @loop\nfi\n\nprint $out";

void openFile(char *fname) {}

char readFile(ibword pos) {
	return 0;
}

bool fileExists(char *fname) {
	return false;
}
void closeFile() {}



#endif
