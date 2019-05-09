//Quit command.
if (compareIgnoreCase(command, "quit")) {
	return ERROR_HALTING;
}

if (compareIgnoreCase(command, "print")) {
	//Verify number of arguments.
	if (arg != 2)
		return ERROR_ARGUMENT_COUNT;
	if (getExprType(argsStart[1], argsSize[1]) == TYPE_NUM) {
		if (!verifyFormula(argsStart[1], argsSize[1]))
			return ERROR_SYNTAX;
		char err = copyFormulaIntoEvalBuff(argsStart[1], argsSize[1]);
		if (err != 0) return err;
		float ans = evaluateFormula();
		printFloat(ans);
		writeChar('\n');
	} else {
		if (!verifyString(argsStart[1], argsSize[1]))
			return ERROR_SYNTAX;
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
		
	if (getExprType(argsStart[1], argsSize[1]) == TYPE_NUM) {
		if (!verifyFormula(argsStart[1], argsSize[1]))
			return ERROR_SYNTAX;
		char err = copyFormulaIntoEvalBuff(argsStart[1], argsSize[1]);
		if (err != 0) return err;
		return ERROR_CHANGE_ADDRESS;
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
		if (!isAlphaNum(outputKey[0]))
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
		if (outputKey[0] != '$')
			return ERROR_INVALID_TYPE;
		char c, outputPos;
		outputPos = 0;
		ibword size = readStrSize(outputAddress);
		c = readChar();
		while (c != '\n' && outputPos != size) {
			if (outputPos < size) {
				if (c != -1)
					writeStr(outputAddress, outputPos++, c);
			}
			c = readChar();
		}
		if (outputPos != size) {
			writeStr(outputAddress, outputPos++, 0);
		}
	} else {
		char c;
		c = readChar();
		while (c != '\n') {
			c = readChar();
		}
	}
	
	return 0;
}

//Runs a file.
if (compareIgnoreCase(command, "run")) {
	if (arg != 2 && arg != 1)
		return ERROR_ARGUMENT_COUNT;
	
	if (arg == 2) {
		if (getExprType(argsStart[1], argsSize[1]) != TYPE_STR)
			return ERROR_INVALID_TYPE;
		char fileName[20];
		char fPos = 0;
		if (!verifyString(argsStart[1], argsSize[1]))
			return ERROR_SYNTAX;
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
		eval(0);
		closeFileOnDevice();
	} else {
		PROGRAM_IN_RAM = 1;
		eval(PROGRAM_LOAD_ADDR);
	}
	return 0;
}

//Loads a file into RAM.
if (compareIgnoreCase(command, "load")) {

	if (arg != 2)
		return ERROR_ARGUMENT_COUNT;
		
	char fileName[20];
	char fPos = 0;
	if (getExprType(argsStart[1], argsSize[1]) != TYPE_STR)
		return ERROR_INVALID_TYPE;
	if (!verifyString(argsStart[1], argsSize[1]))
		return ERROR_SYNTAX;
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
	ibword s = sizeOfFileOnDevice();
	PROGRAM_LOAD_ADDR = sizeRAM() - s - 1;
	printString("Loading ");
	printInt(s + 1);
	printString(" bytes...\n");
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

//Clears variables.
if (compareIgnoreCase(command, "clear")) {
	if (arg != 1)
		return ERROR_ARGUMENT_COUNT;
	AVL_ROOT = 0;
	AVL_END = 0;
	return 0;
}



//Peeks at the RAM.
if (compareIgnoreCase(command, "peek")) {

	if (arg != 2)
		return ERROR_ARGUMENT_COUNT;
	
	if (getExprType(argsStart[1], argsSize[1]) != TYPE_NUM)
		return ERROR_INVALID_TYPE;
	if (!verifyFormula(argsStart[1], argsSize[1]))
		return ERROR_SYNTAX;
	char err = copyFormulaIntoEvalBuff(argsStart[1], argsSize[1]);
	if (err != 0) return err;
	if (pos < 0 || pos >= sizeRAM())
		return ERROR_OUT_OF_BOUNDS;
	ibword pos = (ibword)evaluateFormula();
	/*float num;
	char *ptr = (char*)(&num);
	for (int i = 0; i < sizeof(float); i++)
		ptr[i] = readRAM(pos + i);*/
	if (hasArrow) {
		if (!isAlphaNum(outputKey[0]))
			return ERROR_INVALID_TYPE;
		writeNum(outputAddress, readRAM(pos));
	} else {
		printFloat(readRAM(pos));
		putchar('\n');
	}
	return 0;
}

//Writes to the RAM.
if (compareIgnoreCase(command, "poke")) {
	char err;
	if (arg != 3)
		return ERROR_ARGUMENT_COUNT;
	
	ibword a;
	if (getExprType(argsStart[1], argsSize[1]) != TYPE_NUM)
		return ERROR_INVALID_TYPE;
	if (!verifyFormula(argsStart[1], argsSize[1]))
		return ERROR_SYNTAX;
	err = copyFormulaIntoEvalBuff(argsStart[1], argsSize[1]);
	if (err != 0) return err;
	a = (ibword)evaluateFormula();
	
	//float b;
	char b;
	if (getExprType(argsStart[2], argsSize[2]) != TYPE_NUM)
		return ERROR_INVALID_TYPE;
	if (!verifyFormula(argsStart[2], argsSize[2]))
		return ERROR_SYNTAX;
	err = copyFormulaIntoEvalBuff(argsStart[2], argsSize[2]);
	if (err != 0) return err;
	b = (char)evaluateFormula();
	
	if (a < AVL_END || a >= sizeRAM()/* - sizeof(float)*/)
		return ERROR_OUT_OF_BOUNDS;
	
	/*char *ptr = (char*)(&b);
	for (int i = 0; i < sizeof(float); i++)
		writeRAM(a + i, ptr[i]);*/
	writeRAM(a, b);
	return 0;
}


