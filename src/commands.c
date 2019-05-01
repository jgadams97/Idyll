//Print command
if (compareIgnoreCase(command, "print")) {
	//Verify number of arguments.
	if (arg != 2)
		return ERROR_ARGUMENT_COUNT;
	if (getExprType(argsStart[1], argsSize[1]) == TYPE_NUM) {
		if (!verifyFormula(argsStart[1], argsSize[1]))
			return ERROR_SYNTAX;
		if (!copyFormulaIntoEvalBuff(argsStart[1], argsSize[1]))
			return ERROR_KEY_NOT_FOUND;
		float ans = evaluateFormula();
		printFloat(ans);
		writeChar('\n');
	} else {
		if (!verifyString(argsStart[1], argsSize[1]))
			return ERROR_SYNTAX;
		if (!copyStringIntoEvalBuff(argsStart[1], argsSize[1]))
			return ERROR_KEY_NOT_FOUND;
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
		if (!copyFormulaIntoEvalBuff(argsStart[1], argsSize[1]))
			return ERROR_KEY_NOT_FOUND;
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
		word size = readStrSize(outputAddress);
		c = readChar();
		while (c != '\n' && outputPos != size) {
			if (c != -1)
				writeStr(outputAddress, outputPos++, c);
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
	if (arg != 2)
		return ERROR_ARGUMENT_COUNT;
	if (getExprType(argsStart[1], argsSize[1]) != TYPE_STR)
		return ERROR_INVALID_TYPE;
	
	char fileName[20];
	char fPos = 0;
	if (!verifyString(argsStart[1], argsSize[1]))
		return ERROR_SYNTAX;
	if (!copyStringIntoEvalBuff(argsStart[1], argsSize[1]))
		return ERROR_KEY_NOT_FOUND;
	char c;
	while (c = readCharFromEvalBuff()) {
		if (fPos == 19)
			return ERROR_INVALID_OPTION;
		fileName[fPos++] = c;
	}
	fileName[fPos] = 0;
	
	if (!fileExists(fileName))
		return ERROR_FILE_NOT_FOUND;
		
	openFile(fileName);
	eval(0);
	closeFile();
	
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

//Gets character at index.
if (compareIgnoreCase(command, "$charat")) {
	if (arg != 3)
		return ERROR_ARGUMENT_COUNT;
	if (!hasArrow)
		return 0;	
	if (getExprType(argsStart[1], argsSize[1]) != TYPE_STR)
		return ERROR_INVALID_TYPE;
	if (getExprType(argsStart[2], argsSize[2]) != TYPE_NUM)
		return ERROR_INVALID_TYPE;
	if (!verifyFormula(argsStart[2], argsSize[2]))
		return ERROR_SYNTAX;
	if (!copyFormulaIntoEvalBuff(argsStart[2], argsSize[2]))
		return ERROR_KEY_NOT_FOUND;
	if (outputKey[0] != '$')
		return ERROR_INVALID_TYPE;
		
	int index = (int)evaluateFormula();
	
	char key[KEY_SIZE + 1];
	char i;
	for (i = 0; i < argsSize[1]; i++) {
		key[i] = LINE_BUFF[argsStart[1] + i];
	}
	key[i] = 0;
	
	if (key[0] != '$') 
		return ERROR_INVALID_TYPE;
	if (!verifyKey(key))
		return ERROR_INVALID_KEY;
	word addr = findNode(key);
	if (addr == undefined)
		return ERROR_KEY_NOT_FOUND;
	
	int size_a = readStrSize(addr);
	int size_b = readStrSize(outputAddress);
	
	if (index >= size_a)
		return ERROR_OUT_OF_BOUNDS;
	
	if (size_b == 1) {
		writeStr(outputAddress, 0, readStr(addr, index));
	} else if (size_b > 1) {
		writeStr(outputAddress, 0, readStr(addr, index));
		writeStr(outputAddress, 1, 0);
	} else {
		return ERROR_OUT_OF_BOUNDS;
	}
	
	return 0;
}

//Gets character at index.
if (compareIgnoreCase(command, "charcodeat")) {
	if (arg != 3)
		return ERROR_ARGUMENT_COUNT;
	if (!hasArrow)
		return 0;	
	if (getExprType(argsStart[1], argsSize[1]) != TYPE_STR)
		return ERROR_INVALID_TYPE;
	if (getExprType(argsStart[2], argsSize[2]) != TYPE_NUM)
		return ERROR_INVALID_TYPE;
	if (!verifyFormula(argsStart[2], argsSize[2]))
		return ERROR_SYNTAX;
	if (!copyFormulaIntoEvalBuff(argsStart[2], argsSize[2]))
		return ERROR_KEY_NOT_FOUND;
	if (outputKey[0] == '$' || outputKey[0] == '@')
		return ERROR_INVALID_TYPE;
		
	int index = (int)evaluateFormula();
	
	char key[KEY_SIZE + 1];
	char i;
	for (i = 0; i < argsSize[1]; i++) {
		key[i] = LINE_BUFF[argsStart[1] + i];
	}
	key[i] = 0;
	
	if (key[0] != '$') 
		return ERROR_INVALID_TYPE;
	if (!verifyKey(key))
		return ERROR_INVALID_KEY;
	word addr = findNode(key);
	if (addr == undefined)
		return ERROR_KEY_NOT_FOUND;
	
	int size_a = readStrSize(addr);
	if (index >= size_a)
		return ERROR_OUT_OF_BOUNDS;
	
	writeNum(outputAddress, readStr(addr, index));
	return 0;
}
