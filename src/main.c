#include "idyllic.c"

void main(int argc, char **args) {
	if (argc == 2) {
		if (!fileExistsOnDevice(args[1])) {
			printf("File `%s` not found.\n", args[1]);
			return;
		} else {
			openFileOnDevice(args[1]);
			eval(0);
			closeFileOnDevice();
			return;
		}
	}
	char userInput[100];
	printString("IdyllicBASIC v0.6\n");
	while (1) {
		printString("] ");
		char pos = 0;
		char c = readChar();
		while (c != '\n') {
			userInput[pos++] = c;
			if (pos == 100)
				break;
			c = readChar();
		}
		userInput[pos] = 0;
		if (!copyStringIntoLineBuff(userInput)) {
			printf("Input too long.\n");
			fflush(stdin);
			continue;
		}
		char e;
		if ( (e = evalLine(0, 0)) != 0) {
			if (e == ERROR_HALTING)
				break;
			printError(e);
			printString(".\n");
		}
	}
}
