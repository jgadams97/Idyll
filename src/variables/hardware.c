//This file handles hardware specifics.

//ROM data.
const char ROM[8 * 7 + 1] = {
	'P', 'R', 'I', 'N', 'T',   0,   0,   0,
	'D', 'I', 'M',   0,   0,   0,   0,   0,
	'I', 'N', 'C',   0,   0,   0,   0,   0,
	'A', 'D', 'D',   0,   0,   0,   0,   0,
	'S', 'U', 'B',   0,   0,   0,   0,   0,
	'M', 'U', 'L',   0,   0,   0,   0,   0,
	'D', 'I', 'V',   0,   0,   0,   0,   0,
	-1
};

//Read a byte from RAM.
char readRAM(word pos) {
	return RAM[pos];
}

//Write a byte to RAM. 
void writeRAM(word pos, char b) {
	RAM[pos] = b;
}

//Read a byte from program memory.
char readPRG(word pos) {
	return PRG[pos];
}

//Write a byte to program memory. 
void writePRG(word pos, char b) {
	PRG[pos] = b;
}

//Read a byte from ROM.
char readROM(word pos) {
	return ROM[pos];
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
