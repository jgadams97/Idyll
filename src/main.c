#include "idyllic.c"

void main() {
	char userInput[100];
	printString("IdyllicBASIC v0.1\n");
	while (1) {
		printString("] ");
		char pos = 0;
		char c = readChar();
		while (c != '\n') {
			userInput[pos++] = c;
			c = readChar();
		}
		userInput[pos] = 0;
		copyStringIntoLineBuff(userInput);
		char e;
		if ( (e = evalLine(0, 0)) != 0) {
			if (e == ERROR_HALTING)
				break;
			printError(e);
			printString(".\n");
		}
	}
}
