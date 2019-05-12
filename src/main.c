#include "idyllic.c"

void main(int argc, char **args) {
	if (argc == 2) {
		if (!fileExistsOnDevice(args[1])) {
			printf("File `%s` not found.\n", args[1]);
			return;
		} else {
			openFileOnDevice(args[1]);
			PROGRAM_IN_RAM = 0;
			evalPos(0);
			closeFileOnDevice();
			return;
		}
	}
	char userInput[LINE_BUFF_MAX + 1];
	printString("IdyllicBASIC v0.6\n");
	while (1) {
		printString("] ");
		char pos = 0;
		char c = readChar();
		while (c != '\n') {
			userInput[pos++] = c;
			if (pos == LINE_BUFF_MAX)
				break;
			c = readChar();
		}
		if (pos != LINE_BUFF_MAX) {
			userInput[pos] = 0;
			char e;
			if ( (e = eval(userInput)) != 0 ) {
				if (e == ERROR_HALTING)
					break;
				printError(e);
				printString(".\n");
			}
		} else {
			printString("Too long.\n");
		}
	}
}
