#include "idyllic_dynamic.c"

void main(int argc, char **args) {
	Evaluator e;
	if (argc == 2) {
		if (!fileExistsOnDevice(args[1])) {
			printf("File `%s` not found.\n", args[1]);
			return;
		} else {
			openFileOnDevice(args[1]);
			e.in_ram = 0;
			evalPos(&e, 0);
			closeFileOnDevice();
			return;
		}
	}
	char userInput[LINE_BUFF_MAX + 1];
	printString("IdyllicBASIC v0.7\n");
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
			char err;
			if ( (err = eval(&e, userInput)) != 0 ) {
				if (err == ERROR_HALTING)
					break;
				printError(err);
				printString(".\n");
			}
		} else {
			printString("Too long.\n");
		}
	}
}
