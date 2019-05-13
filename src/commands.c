
#ifdef DESKTOP
//Quit command.
if (compareIgnoreCase(command, "quit")) {
	return ERROR_HALTING;
}
#endif

if (compareIgnoreCase(command, "print")) {
	//Verify number of arguments.
	if (arg != 2)
		return ERROR_ARGUMENT_COUNT;
	if (argsType[1] == TYPE_NUM) {
		char err = copyFormulaIntoEvalBuff(argsStart[1], argsSize[1]);
		if (err != 0) return err;
		float ans = evaluateFormula();
		printFloat(ans);
		writeChar('\n');
	} else {
		char err = copyStringIntoEvalBuff(argsStart[1], argsSize[1]);
		if (err != 0) return err;
		char c;
		bool isEscape = false;
		while (c = readCharFromEvalBuff()) {
			if (c == '\\' && !isEscape) {
				isEscape = true;
				continue;
			} else if (isEscape) {
				if (c == 'n' || c == 'N')
					writeChar('\n');
				if (c == '\\')
					writeChar('\\');
				isEscape = false;
			} else {
				writeChar(c);
			}
		}
		writeChar('\n');
	}
	return 0;
}
	
//GOTO Command.
if (compareIgnoreCase(command, "goto")) {
	//Verify number of arguments.
	if (arg != 2)
		return ERROR_ARGUMENT_COUNT;
		
	if (argsType[1] == TYPE_NUM) {
		char err = copyFormulaIntoEvalBuff(argsStart[1], argsSize[1]);
		if (err != 0) return err;
		return ERROR_CHANGE_ADDRESS;
	} else {
		return ERROR_INVALID_TYPE;
	}
	
	return 0;
}

//GOSUB Command.
if (compareIgnoreCase(command, "gosub")) {
	//Verify number of arguments.
	if (arg != 2)
		return ERROR_ARGUMENT_COUNT;
		
	if (argsType[1] == TYPE_NUM) {
		char err = copyFormulaIntoEvalBuff(argsStart[1], argsSize[1]);
		if (err != 0) return err;
		return ERROR_CHANGE_ADDRESS_CALL;
	} else {
		return ERROR_INVALID_TYPE;
	}
	
	return 0;
}

//FREE RAM Command.
if (compareIgnoreCase(command, "free")) {
	
	//Verify number of arguments.
	if (arg != 1)
		return ERROR_ARGUMENT_COUNT;
	
	//Needs output.
	if (hasArrow) {
		if (!isAlpha(outputKey[0]))
			return ERROR_INVALID_TYPE;
		writeNum(outputAddress, sizeRAM() - AVL_END);
	} else {
		//No output? Just print it.
		if (!hasArrow) {
			printFloat(sizeRAM() - AVL_END);
			printString(" bytes free.\n");
			return 0;
		}
	}
	
	return 0;
}

//Reads a line of text from the user.
if (compareIgnoreCase(command, "$read")) {
	//Needs output.
	if (hasArrow) {
		char pos = 0;
		for (char i = 0; i < KEY_SIZE && outputKey[pos] != 0; i++) {
			LINE_BUFF[pos++] = outputKey[i];
		}
		LINE_BUFF[pos++] = '=';
		LINE_BUFF[pos++] = '\"';
		char c = readChar();
		while (c != '\n') {
			if (c != (char)-1 && c != 0 && c != 0x08) {
				LINE_BUFF[pos++] = c == '\"' ? '\'' : c;
			} else if (c == 0x08 && pos != 0) {
				#ifdef ARDUINO
				backspace();
				#endif
				pos--;
			}
			c = readChar();
		}
		LINE_BUFF[pos++] = '\"';
		LINE_BUFF[pos++] = '\n';
		return evalAssignment(NULL);
		
	} else {
		char c;
		c = readChar();
		while (c != '\n') {
			c = readChar();
		}
	}
	
	return 0;
}

#ifdef DESKTOP
//Runs a file.
if (compareIgnoreCase(command, "run")) {
	if (arg != 2 && arg != 1)
		return ERROR_ARGUMENT_COUNT;
	
	if (arg == 2) {
		if (argsType[1] != TYPE_STR)
			return ERROR_INVALID_TYPE;
		char fileName[20];
		char fPos = 0;
		char err = copyStringIntoEvalBuff(argsStart[1], argsSize[1]);
		if (err != 0) return err;
		char c;
		while (c = readCharFromEvalBuff()) {
			if (fPos == 19)
				return ERROR_INVALID_OPTION;
			fileName[fPos++] = c;
		}
		fileName[fPos] = 0;
		
		if (!fileExistsOnDevice(fileName))
			return ERROR_FILE_NOT_FOUND;

		openFileOnDevice(fileName);
		PROGRAM_IN_RAM = 0;
		evalPos(0);
		closeFileOnDevice();
	} else {
		PROGRAM_IN_RAM = 1;
		evalPos(PROGRAM_LOAD_ADDR);
	}
	return 0;
}
#endif

#ifdef ARDUINO

//Runs a file.
if (compareIgnoreCase(command, "run")) {
	if (arg != 1)
		return ERROR_ARGUMENT_COUNT;
	
	PROGRAM_IN_RAM = 1;
	evalPos(PROGRAM_LOAD_ADDR);
	return 0;
}
#endif
//Loads a file into RAM.
if (compareIgnoreCase(command, "load")) {

	if (arg != 2)
		return ERROR_ARGUMENT_COUNT;
		
	char fileName[20];
	char fPos = 0;
	if (argsType[1] != TYPE_STR)
		return ERROR_INVALID_TYPE;
	char err = copyStringIntoEvalBuff(argsStart[1], argsSize[1]);
	if (err != 0) return err;
	char c;
	while (c = readCharFromEvalBuff()) {
		#ifdef ARDUINO
		if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
		#endif
		if (fPos == 19)
			return ERROR_INVALID_OPTION;
		fileName[fPos++] = c;
	}
	fileName[fPos] = 0;
	
	if (!fileExistsOnDevice(fileName))
		return ERROR_FILE_NOT_FOUND;
	
	openFileOnDevice(fileName);
	ibword s = sizeOfFileOnDevice();
	PROGRAM_LOAD_ADDR = sizeRAM() - s - 1;
	printString("Loading ");
	printInt(s + 1);
	printString(" bytes.\n");
	for (ibword i = 0; i < s; i++) {
		char rc = readFileOnDevice(i);
		writeRAM(PROGRAM_LOAD_ADDR + i, rc);
		//lcdPutChar(rc);
	}
	writeRAM(PROGRAM_LOAD_ADDR + s, 0);
	printString("Done.\n");
	closeFileOnDevice();
	
	return 0;
}

#ifdef ARDUINO

//Loads a file into RAM.
if (compareIgnoreCase(command, "edit")) {

	if (arg != 2)
		return ERROR_ARGUMENT_COUNT;
		
	char fileName[9];
	char fPos = 0;
	if (argsType[1] != TYPE_STR)
		return ERROR_INVALID_TYPE;
	char err = copyStringIntoEvalBuff(argsStart[1], argsSize[1]);
	if (err != 0) return err;
	char c;
	while (c = readCharFromEvalBuff()) {
		if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
		if (fPos == 8)
			return ERROR_INVALID_OPTION;
		fileName[fPos++] = c;
	}
	fileName[fPos] = 0;
	
	ibword newSize = -1;
	if (!fileExistsOnDevice(fileName)) {
		newSize = linesEditor(0, 4, 20);
	} else {
		openFileOnDevice(fileName);
		ibword s = sizeOfFileOnDevice();
		unsigned short x = 0, y = 0;
		PROGRAM_LOAD_ADDR = sizeRAM() - s - 1;
		printString("Loading ");
		printInt(s);
		printString(" bytes.\n");
		for (ibword i = 0; i < s; i++) {
			char rc = readFileOnDevice(i);
			if (rc == '\n') {
				writeRAM(x + y * LINE_BUFF_MAX, 0);
				x = 0;
				y++;
			} else {
				writeRAM(x + y * LINE_BUFF_MAX, rc);
				x++;
			}
		}
		writeRAM(y * LINE_BUFF_MAX, 0);
		printString("Done.\n");
		closeFileOnDevice();
		newSize = linesEditor(y, 4, 20);
	}
	
	if (newSize != -1) {
		File oldFile = openFile(NULL, fileName);
		if (oldFile.status == 0)
			deleteFile(&oldFile);
		if (newSize > 4) {
			printString("Saving...\n");
			File newFile = createFile(newSize + 1, NULL, fileName);
			seekFile(&newFile, 0);
			unsigned short row = 0;
			unsigned short col = 0;
			for (unsigned short i = 0; i < newSize; i++) {
				char c = readRAM(row * LINE_BUFF_MAX + col);
				if (c == 0) {
					writeFile(&newFile, '\n');
					col = 0;
					row++;
				} else {
					writeFile(&newFile, c);
					col++;
				}
			}
			writeFile(&newFile, 0);
			printString("Done.\n");
		}
	}
		
	AVL_ROOT = 0;
	AVL_END = 0;
	return 0;
}

if (compareIgnoreCase(command, "files")) {
	if (arg != 1)
		return ERROR_ARGUMENT_COUNT;

	char name[20];
	char fpos = 0;
	while (listFilesInFolder(NULL, name)) {
		printString(name);
		printString("...");
		File f = openFile(NULL, name);
		printFloat(f.size);
		fpos++;
		if (fpos == 4) {
			readChar();
			backspace();
			fpos = 0;
		}
		printString("\n");
	}
	return 0;
}


//Loads a file into RAM.
if (compareIgnoreCase(command, "delete")) {

	if (arg != 2)
		return ERROR_ARGUMENT_COUNT;
		
	char fileName[20];
	char fPos = 0;
	if (argsType[1] != TYPE_STR)
		return ERROR_INVALID_TYPE;
	char err = copyStringIntoEvalBuff(argsStart[1], argsSize[1]);
	if (err != 0) return err;
	char c;
	while (c = readCharFromEvalBuff()) {
		if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
		if (fPos == 19)
			return ERROR_INVALID_OPTION;
		fileName[fPos++] = c;
	}
	fileName[fPos] = 0;
	
	if (!fileExistsOnDevice(fileName))
		return ERROR_FILE_NOT_FOUND;
		
	File file = openFile(NULL, fileName);
	deleteFile(&file);
	
	return 0;
}

if (compareIgnoreCase(command, "random")) {
	if (arg != 1)
		return ERROR_ARGUMENT_COUNT;
	
	unsigned short seed = KB_RANDOM * 49627 + 49627;
	float rnd = (float)seed / (float)65535;
	
	if (hasArrow) {
		if (!isAlpha(outputKey[0]))
			return ERROR_INVALID_TYPE;
		writeNum(outputAddress, rnd);
	} else {
		printFloat(rnd);
		printString("\n");
	}
	
	return 0;
}

//Clear screen.
if (compareIgnoreCase(command, "cls")) {
	if (arg != 1)
		return ERROR_ARGUMENT_COUNT;
	lcdClear();
	return 0;
}

#endif


if (compareIgnoreCase(command, "floor")) {
	if (arg != 2)
		return ERROR_ARGUMENT_COUNT;
	
	if (argsType[1] != TYPE_NUM)
		return ERROR_INVALID_TYPE;
		
	char err = copyFormulaIntoEvalBuff(argsStart[1], argsSize[1]);
	if (err != 0) return err;
	
	float num = (unsigned long)evaluateFormula();
	
	if (hasArrow) {
		if (!isAlpha(outputKey[0]))
			return ERROR_INVALID_TYPE;
		writeNum(outputAddress, num);
	} else {
		printFloat(num);
		printString("\n");
	}
	
	return 0;
}


//Converts a string into a number.
if (compareIgnoreCase(command, "value")) {

	if (arg != 2)
		return ERROR_ARGUMENT_COUNT;
		
	char str[21];
	char strPos = 0;
	if (argsType[1] != TYPE_STR)
		return ERROR_INVALID_TYPE;
	char err = copyStringIntoEvalBuff(argsStart[1], argsSize[1]);
	if (err != 0) return err;
	char c;
	while (c = readCharFromEvalBuff()) {
		if (strPos == 20) break;
		str[strPos++] = c;
	}
	str[strPos] = 0;
	float val = atof(str);
	
	if (hasArrow) {
		if (!isAlpha(outputKey[0]))
			return ERROR_INVALID_TYPE;
		writeNum(outputAddress, val);
	} else {
		printFloat(val);
		printString("\n");
	}
	
	return 0;
}

//Clears variables.
if (compareIgnoreCase(command, "clear")) {
	if (arg != 1)
		return ERROR_ARGUMENT_COUNT;
	AVL_ROOT = 0;
	AVL_END = 0;
	return 0;
}

#ifndef ARDUINO
//Peeks at the RAM.
if (compareIgnoreCase(command, "peek")) {

	if (arg != 2)
		return ERROR_ARGUMENT_COUNT;
	
	if (argsType[1] != TYPE_NUM)
		return ERROR_INVALID_TYPE;
	char err = copyFormulaIntoEvalBuff(argsStart[1], argsSize[1]);
	if (err != 0) return err;
	if (pos < 0 || pos >= sizeRAM())
		return ERROR_OUT_OF_BOUNDS;
	ibword pos = (ibword)evaluateFormula();
	
	if (hasArrow) {
		if (!isAlpha(outputKey[0]))
			return ERROR_INVALID_TYPE;
		writeNum(outputAddress, readRAM(pos));
	} else {
		printFloat(readRAM(pos));
		printString("\n");
	}
	return 0;
}

//Writes to the RAM.
if (compareIgnoreCase(command, "poke")) {
	char err;
	if (arg != 3)
		return ERROR_ARGUMENT_COUNT;
	
	ibword a;
	if (argsType[1] != TYPE_NUM)
		return ERROR_INVALID_TYPE;
	err = copyFormulaIntoEvalBuff(argsStart[1], argsSize[1]);
	if (err != 0) return err;
	a = (ibword)evaluateFormula();
	
	//float b;
	char b;
	if (argsType[2] != TYPE_NUM)
		return ERROR_INVALID_TYPE;
	err = copyFormulaIntoEvalBuff(argsStart[2], argsSize[2]);
	if (err != 0) return err;
	b = (char)evaluateFormula();
	
	if (a < AVL_END || a >= sizeRAM())
		return ERROR_OUT_OF_BOUNDS;
	
	writeRAM(a, b);
	return 0;
}
#endif