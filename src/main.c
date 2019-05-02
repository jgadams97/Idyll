#include "idyllic.c"

void main(int argc, char **args) {
	if (argc == 2) {
		if (!fileExists(args[1])) {
			printf("File `%s` not found.\n", args[1]);
			return;
		} else {
			openFile(args[1]);
			eval(0);
			closeFile();
			return;
		}
	}
	char userInput[100];
	printString("IdyllicBASIC v0.2\n");
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
