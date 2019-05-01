#include "parsing/evaluator.h"
char eval(ibword pos);
/*
dim x
x = 0
@loop
	x = x + 1
	print x
goto @loop

dim $bob[10]
$bob = "hello"
print $bob

*/

//Returns -1 if line is too long.
//	Otherwise returns offset.

void copyStringIntoLineBuff(char *s) {
	ibword i = 0;
	char c = s[0];
	while (!isEOL(c)) {
		LINE_BUFF[i] = c;
		c = s[++i];
		if (i == LINE_BUFF_MAX)
			return;
	}
	LINE_BUFF[i] = 0;
}

ibword copyFileIntoLineBuff(ibword pos) {
	ibword i = 0;
	char c = readFile(pos);
	while (!isEOL(c)) {
		LINE_BUFF[i++] = c;
		pos += 1;
		if (i == LINE_BUFF_MAX)
			return -1;
			
		c = readFile(pos);
		
	}
	LINE_BUFF[i] = 0;
	return i + 1;
}

//Evaluate a reference statement.
//	0	No Error.
char evalReference(ibword addr) {
	ibword pos = 0;
	char c;
	char key[KEY_SIZE + 1];
	char p = 0;
	
	//Skip over garbage whitespace.
	c = LINE_BUFF[pos];
	while (isWS(c)) {
		c = LINE_BUFF[++pos];
	}
	
	//Load the key.
	while (!isWS(c) && !isEOL(c)) {
		key[p++] = c;
		c = LINE_BUFF[++pos];
		if (p > KEY_SIZE)
			return ERROR_INVALID_KEY;
	}
	key[p] = 0;
	
	//Check if there is more garbage text afterwards.
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	if (!isEOL(c))
		return ERROR_SYNTAX;
	if (!verifyKey(key))
		return ERROR_INVALID_KEY;
	
	//Check if the key exists.
	if (findNode(key) != undefined)
		return ERROR_KEY_EXISTS;
	
	//No issues. Create the key.
	dimAdr(key, addr);
	
	return 0;
}

//Evaluate an assignment.
//	0	No error.
char evalAssignment() {
	char c;
	char key[KEY_SIZE];
	char keyPos = 0;
	ibword pos = 0;
	
	//Skip over white space.
	c = LINE_BUFF[pos];
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	
	//Syntax error.
	if (isEOL(c)) return 1;
	
	//Read in key.
	if (c == '$') {
		key[keyPos++] = c;
		c = LINE_BUFF[++pos];
	}
	while (isAlphaNum(c) && !isEOL(c)) {
		key[keyPos++] = c;
		if (keyPos > KEY_SIZE)
			return ERROR_INVALID_KEY;
		c = LINE_BUFF[++pos];
	}
	key[keyPos] = 0;
	
	//Syntax error.
	if (isEOL(c)) return ERROR_SYNTAX;
	
	//Key doesn't exist.
	if (!verifyKey(key)) return ERROR_SYNTAX;
	
	//Check if key exists.
	ibword addr = findNode(key);
	if (addr == undefined)
		return ERROR_KEY_NOT_FOUND;
		
	//Skip to equal sign.
	while (c != '=' && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	
	//Syntax error.
	if (isEOL(c))
		return ERROR_SYNTAX;
	
	//Find start and size.
	c = LINE_BUFF[++pos];
	ibword start = pos;
	ibword size = 0;
	while (!isEOL(c)) {
		c = LINE_BUFF[++pos];
		size++;
	}
	
	//Copy formula to eval buff.
	if (isAlpha(key[0])) {
		if (!verifyFormula(start, size))
			return ERROR_SYNTAX;
		if (!copyFormulaIntoEvalBuff(start, size))
			return ERROR_SYNTAX;
		writeNum(addr, evaluateFormula());
	} else if (key[0] == '$') {
		if (!verifyString(start, size))
			return ERROR_SYNTAX;
		if (!copyStringIntoEvalBuff(start, size))
			return ERROR_SYNTAX;
		ibword strSize = readStrSize(addr);
		ibword strPos = 0;
		char c = readCharFromEvalBuff();
		writeStr(addr, strPos++, c);
		while (c = readCharFromEvalBuff()) {
			if (strPos == strSize)
				return ERROR_OUT_OF_BOUNDS;
			writeStr(addr, strPos++, c);
		}
		if (strPos != strSize) {
			writeStr(addr, strPos, 0);
		}
	} else {
		//Syntax error.
		return ERROR_SYNTAX;
	}
	
	return 0;
}

//Evaluate a conditional.
//	0	No error.
char evalUnconditional() {
	char c;
	ibword pos = 0;
	
	//Skip over white space.
	c = LINE_BUFF[pos];
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	
	//Syntax error.
	if (isEOL(c) || (c != 'f' && c != 'F'))
		return ERROR_SYNTAX;
	c = LINE_BUFF[++pos];
	if (isEOL(c) || (c != 'i' && c != 'i'))
		return ERROR_SYNTAX;
		
	//Skip over white space.
	c = LINE_BUFF[++pos];
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	
	//Syntax error.
	if (!isEOL(c))
		return ERROR_SYNTAX;
		
	return 0;
}

//Evaluate a conditional.
//	0	No error.
char evalConditional() {
	ibword lhsStart = 0;
	ibword lhsSize = 0;
	ibword rhsStart = 0;
	ibword rhsSize = 0;
	char cond1 = ' ';
	char cond2 = ' ';
	char c;
	char pos = 0;
	
	//Skip over white space.
	c = LINE_BUFF[pos];
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	
	//Syntax error.
	if (isEOL(c))
		return ERROR_SYNTAX;
	
	//Skip over command.
	while (!isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	
	//Syntax error.
	if (isEOL(c))
		return ERROR_SYNTAX;
		
	//Skip over white space.
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	
	//Get LHS.
	lhsStart = pos;
	while (!isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
		lhsSize++;
	}
	
	//Syntax error.
	if (isEOL(c))
		return ERROR_SYNTAX;
	
	//Skip over white space.
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	
	//Syntax error.
	if (isEOL(c))
		return ERROR_SYNTAX;
	
	//Get the first conditional.
	cond1 = c;
	c = LINE_BUFF[++pos];
	
	//Syntax error.
	if (isEOL(c))
		return ERROR_SYNTAX;
		
	//Get the second conditional.
	if (!isWS(c)) {
		cond2 = c;
		c = LINE_BUFF[++pos];
	}
	
	//Syntax error.
	if (isEOL(c) || !isWS(c))
		return ERROR_SYNTAX;
	
	//Skip over white space.
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	
	//Get RHS.
	rhsStart = pos;
	while (!isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
		rhsSize++;
	}
	
	//Skip over white space.
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	
	//Check for "THEN"
	if (isEOL(c) || (c != 't' && c != 'T'))
		return ERROR_MISSING_THEN;
	c = LINE_BUFF[++pos];
	if (isEOL(c) || (c != 'h' && c != 'H'))
		return ERROR_MISSING_THEN;
	c = LINE_BUFF[++pos];
	if (isEOL(c) || (c != 'e' && c != 'E'))
		return ERROR_MISSING_THEN;
	c = LINE_BUFF[++pos];
	if (isEOL(c) || (c != 'n' && c != 'N'))
		return ERROR_MISSING_THEN;
	c = LINE_BUFF[++pos];
	
	//Skip over whitespace.
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	
	//Syntax error.
	if (!isEOL(c))
		return ERROR_SYNTAX;
		
	//Check conditional.
	char lhsType = getExprType(lhsStart, lhsSize);
	char rhsType = getExprType(rhsStart, rhsSize);
	if (lhsType == TYPE_NUM && rhsType == TYPE_NUM) {
		float lhs, rhs;
		if (!verifyFormula(lhsStart, lhsSize))
			return ERROR_SYNTAX;
		if (!copyFormulaIntoEvalBuff(lhsStart, lhsSize))
			return ERROR_SYNTAX;
		lhs = evaluateFormula();
		if (!verifyFormula(rhsStart, rhsSize))
			return ERROR_SYNTAX;
		if (!copyFormulaIntoEvalBuff(rhsStart, rhsSize))
			return ERROR_SYNTAX;
		rhs = evaluateFormula();
		//Check the conditions.
		if (cond1 == '>' && cond2 == ' ') {
			if (!(lhs > rhs))
				return ERROR_CONDITIONAL_TRIGGERED;
		} else if (cond1 == '>' && cond2 == '=') {
			if (!(lhs >= rhs))
				return ERROR_CONDITIONAL_TRIGGERED;
		} else if (cond1 == '<' && cond2 == ' ') {
			if (!(lhs < rhs))
				return ERROR_CONDITIONAL_TRIGGERED;
		} else if (cond1 == '<' && cond2 == '=') {
			if (!(lhs <= rhs))
				return ERROR_CONDITIONAL_TRIGGERED;
		} else if (cond1 == '=' && cond2 == '=') {
			if (!(lhs == rhs))
				return ERROR_CONDITIONAL_TRIGGERED;
		} else if (cond1 == '!' && cond2 == '=') {
			if (!(lhs != rhs))
				return ERROR_CONDITIONAL_TRIGGERED;
		} else {
			return ERROR_SYNTAX;
		}
	} else if (lhsType == TYPE_STR || rhsType == TYPE_STR) {
		char compSize = 8;
		char compA[compSize + 1];
		char compB[compSize + 1];
		char compPosA = 0;
		char compPosB = 0;
		char c;
		
		//Load string A.
		if (!verifyString(lhsStart, lhsSize))
			return ERROR_SYNTAX;
		if (!copyStringIntoEvalBuff(lhsStart, lhsSize))
			return ERROR_SYNTAX;	
		while (c = readCharFromEvalBuff()) {
			compA[compPosA++] = c;
			if (compPosA == compSize)
				break;
		}
		compA[compPosA] = 0;
	
		//Load string B.
		if (!verifyString(rhsStart, rhsSize))
			return ERROR_SYNTAX;
		if (!copyStringIntoEvalBuff(rhsStart, rhsSize))
			return ERROR_SYNTAX;	
		while (c = readCharFromEvalBuff()) {
			compB[compPosB++] = c;
			if (compPosB == compSize)
				break;
		}
		compB[compPosB] = 0;
		
		//Check the conditions.
		if (cond1 == '=' && cond2 == '=') {
			if (strcmp(compA, compB) != 0)
				return ERROR_CONDITIONAL_TRIGGERED;
		} else if (cond1 == '!' && cond2 == '=') {
			if (strcmp(compA, compB) == 0)
				return ERROR_CONDITIONAL_TRIGGERED;
		} else {
			return ERROR_SYNTAX;
		}
	}
	
	return ERROR_CONDITIONAL_UNTRIGGERED;
	
	
}
//Evaluate a declaration.
//	0	No error.
char evalDeclaration() {
	char c;
	char key[KEY_SIZE];
	char keyPos = 0;
	ibword pos = 0;
	char ret = 0;
	bool containsAssignment = false;
	
	//Skip over white space.
	c = LINE_BUFF[pos];
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	//Syntax error.
	if (isEOL(c))
		return ERROR_SYNTAX;
	
	//Skip over "dim".
	pos += 3;
	
	//Syntax error.
	c = LINE_BUFF[pos];
	if (!isWS(c) || isEOL(c))
		return ERROR_SYNTAX;
		
	//Skip over white space.
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	
	//Syntax error.
	if (isEOL(c))
		return ERROR_SYNTAX;
		
	//Read in key.
	key[keyPos++] = c;
	c = LINE_BUFF[++pos];
	while (!isWS(c) && !isEOL(c) && c != '[' && c != '=') {
		key[keyPos++] = c;
		if (keyPos > KEY_SIZE)
			return ERROR_INVALID_KEY;
		c = LINE_BUFF[++pos];
	}
	key[keyPos] = 0;

	//Check to see if variable name is valid.
	if (!verifyKey(key))
		return ERROR_INVALID_KEY;
	
	//Make sure the variable isn't an address.
	if (key[0] == '@')
		return ERROR_INVALID_TYPE;
	
	//Check to see if variable already exists.
	if (findNode(key) != undefined)
		return ERROR_KEY_EXISTS;
	
	//Check to see variable type.
	if (key[0] == '$') {
		//Syntax error.
		char numBuff[11];
		char numPos = 0;
		ibword size = 0;
		if (c != '[')
			return ERROR_SYNTAX;
			
		//Read in size.
		c = LINE_BUFF[++pos];
		while (isNum(c) && !isEOL(c)) {
			numBuff[numPos++] = c;
			c = LINE_BUFF[++pos];
		}
		numBuff[numPos] = 0;
		size = atoi(numBuff);
		
		//Syntax error.
		if (c != ']')
			return ERROR_SYNTAX;
		c = LINE_BUFF[++pos];
		
		//Skip over white space.
		while (isWS(c) && !isEOL(c)) {
			c = LINE_BUFF[++pos];
		}
	
		//Syntax error.
		if (!isEOL(c) && c != '=')
			return ERROR_SYNTAX;
		else if (c == '=')
			containsAssignment = true;
		
		//Make sure the string isn't too massive.
		if (size > STRING_SIZE_MAX)
			return ERROR_STRING_TOO_LARGE;
			
		//Create the variable.
		dimStr(key, size); 

	} else {
		//Skip over white space.
		while (isWS(c) && !isEOL(c)) {
			c = LINE_BUFF[++pos];
		}
		
		//Syntax error.
		if (!isEOL(c) && c != '=')
			ERROR_SYNTAX;
		else if (c == '=')
			containsAssignment = true;
		
		//Create the variable.
		dimNum(key, 0);
		

	}
	
	
	if (containsAssignment) {
		//Skip over white space;
		pos = 0;
		c = LINE_BUFF[pos++];
		while (isWS(c) && !isEOL(c)) {
			c = LINE_BUFF[pos++];
		}
		//Remove "DIM".
		LINE_BUFF[pos - 1] = ' ';
		LINE_BUFF[pos] = ' ';
		LINE_BUFF[pos + 1] = ' ';
		
		//Evaluate assignment.
		ret = evalAssignment();
		
	}

	return ret;
}

bool compareIgnoreCase(const char *a, const char *b) {
	char ca = a[0];
	char cb = b[0];
	char p = 0;
	while (ca != 0 && cb != 0) {
		char d = ('a' - 'A');
		if (
			ca + d != cb &&
			ca - d != cb &&
			ca != cb
			) {
			return false;	
		}
		p++;
		ca = a[p];
		cb = b[p];
	}
	if (ca == 0 && cb == 0)
		return true;
	return false;
}

//Evaluates a command.
char evalCommand() {

	char outputKey[KEY_SIZE + 1];
	ibword outputAddress = undefined;
	ibword argsStart[10];
	ibword argsSize[10];
	char arg = 1;
	char c;
	
	char command[10];
	char commandPos = 0;
	ibword pos = 0;
	bool isInQuotes = false;
	
	//Get start of command.
	c = LINE_BUFF[pos];
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	argsStart[0] = pos;
	
	//Get end of command.
	while (!isWS(c) && !isEOL(c) ) {
		if (commandPos == 10)
			return ERROR_UNKNOWN_COMMAND;
		command[commandPos++] = c;
		c = LINE_BUFF[++pos];
	}
	command[commandPos] = 0;
	argsSize[0] = pos - argsStart[0];
	
	//Get arguments.
	while (!isEOL(c)) {
		//Get start of argument.
		while ( (isWS(c) || c == ',') && !isEOL(c)) {
			c = LINE_BUFF[++pos];
		}
		argsStart[arg] = pos;
		
		//Get end of argument.
		while ( (c != ',' || isInQuotes) && !isEOL(c) ) {
			if (c == '\"') {
				isInQuotes = !isInQuotes;
			}
			c = LINE_BUFF[++pos];
		}
		argsSize[arg] = pos - argsStart[arg];
		
		//Update argument.
		arg++;
	}
	
	//Check for arrow.
	bool hasArrow = false;
	if (arg >= 2) {
		if (LINE_BUFF[argsStart[arg - 2]] == '=' &&
			LINE_BUFF[argsStart[arg - 2]] == '>') {
			arg -= 2;
		}
		
		c = LINE_BUFF[--pos];
		while (pos != argsSize[0]) {
			if (c == '\"') break;
			if (c == '>') {
				c = LINE_BUFF[--pos];
				if (c == '=') {
					hasArrow = true;
					break;
				}
			}
			c = LINE_BUFF[--pos];
		}
	}
	
	
	//Remove arrow from arguments.
	if (hasArrow) {
		//Remove arrow from arguments.
		argsSize[arg - 1] = pos - argsStart[arg - 1] - 1;
		arg--;
		
		//Skip over whitespace.
		pos += 2;
		c = LINE_BUFF[pos++];
		while (isWS(c) && !isEOL(c)) {
			c = LINE_BUFF[pos++];
		}
		
		//Syntax error.
		if (isEOL(c)) 
			return ERROR_SYNTAX;
		
		//Read in key.
		char keyPos = 0;
		while (!isWS(c) && !isEOL(c)) {
			outputKey[keyPos++] = c;
			c = LINE_BUFF[pos++];
		}
		outputKey[keyPos] = 0;
		
		//Skip over whitespace.
		while (isWS(c) && !isEOL(c)) {
			c = LINE_BUFF[pos++];
		}
		
		//Syntax error.
		if (!isEOL(c))
			return ERROR_SYNTAX;
		
		//Check if the key is valid.
		if (!verifyKey(outputKey))
			return ERROR_INVALID_KEY;
			
		//Check if variable exists.
		outputAddress = findNode(outputKey);
		if (outputAddress == undefined)
			return ERROR_KEY_NOT_FOUND;
		
	}
	
	#include "commands.c"
	
	return ERROR_UNKNOWN_COMMAND;

}

//Identifies the type of line.
char identifyLineType() {
	ibword pos = 0;
	bool checkedM = false;
	bool isDeclaration = true;
	bool isConditional = true;
	bool isUnconditional = true;
	bool isComment = true;
	char c = LINE_BUFF[pos];
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	if (isEOL(c)) return TYPE_EMPTY;
	if (c == '@') return TYPE_REFERENCE;
	if (c != '/') isComment = false;
	if ( (!isAlpha(c)) && c != '$' )
		return TYPE_COMMAND;
	if (c != 'D' && c != 'd')
		isDeclaration = false;
	if (c != 'I' && c != 'i')
		isConditional = false;
	if (c != 'f' && c != 'f')
		isUnconditional = false;
	c = LINE_BUFF[++pos];
	if (c != '/') isComment = false;
	if (isComment)
		return TYPE_COMMENT;
	if (c != 'I' && c != 'i')
		isDeclaration = false;
	if (c != 'F' && c != 'f')
		isConditional = false;
	if (c != 'I' && c != 'i')
		isUnconditional = false;
	char nc = LINE_BUFF[pos + 1];
	if (isConditional && (isWS(nc) || isEOL(nc)))
		return TYPE_CONDITIONAL;
	if (isUnconditional && (isWS(nc) || isEOL(nc)))
		return TYPE_UNCONDITIONAL;
	while (isAlphaNum(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
		
		if (!checkedM) {
			nc = LINE_BUFF[pos + 1];
			if ((c != 'M' && c != 'm') || !isWS(nc))
				isDeclaration = false;
		}
		checkedM = true;
	}
	if (isDeclaration && c != '=') return TYPE_DECLARATION;
	if (isEOL(c)) return TYPE_COMMAND;
	while (isWS(c) && !isEOL(c)) {
		c = LINE_BUFF[++pos];
	}
	if (isEOL(c)) return TYPE_COMMAND;
	if (c == '=') {
		c = LINE_BUFF[++pos];
		if (c != '>') return TYPE_ASSIGNMENT;
	}
	return TYPE_COMMAND;
}

//Evaluates a single line at a position.
//	The addr should be the address of the next line.
char evalLine(ibword pos, ibword addr) {
	char type = identifyLineType();
	char ret = 0;
	if (type == TYPE_DECLARATION) {
		ret = evalDeclaration();
	} else if (type == TYPE_ASSIGNMENT) {
		ret = evalAssignment();
	} else if (type == TYPE_REFERENCE) {
		ret = evalReference(addr);
	} else if (type == TYPE_COMMAND) {
		ret = evalCommand();
	} else if (type == TYPE_CONDITIONAL) {
		ret = evalConditional();
	} else if (type == TYPE_UNCONDITIONAL) {
		ret = evalUnconditional();
	}
	return ret;
}

//Pribwords an error message associated
//	with an error code.
void printError(char e) {

	switch (e) {
		case 1:
			printString("Syntax error");
			break;
		case 2:
			printString("Variable not found");
			break;
		case 3:
			printString("Invalid variable name");
			break;
		case 4:
			printString("Variable already exists");
			break;
		case ERROR_OUT_OF_BOUNDS:
			printString("String out of bounds");
			break;
		case 6:
			printString("Invalid type");
			break;
		case 7:
			printString("Invalid argument count");
			break;
		case 10:
			printString("Invalid comparison");
			break;
		case ERROR_MISSING_THEN:
			printString("Missing THEN");
			break;
		case ERROR_STRING_TOO_LARGE:
			printString("String declaration is too large");
			break;
		case ERROR_UNKNOWN_COMMAND:
			printString("Unknown command");
			break;
		case ERROR_FILE_NOT_FOUND:
			printString("File not found");
			break;
		default:
			printString("This command cannot be executed in DIRECT mode");
			break;
	}
	
}

//Evaluates a program at position.
char eval(ibword pos) {
	ibword size;
	char eof = 10;
	ibword skipMode = 0;
	
	while (eof == 10) {
		size = copyFileIntoLineBuff(pos);
		eof = readFile(pos + size - 1);
		char err;
		if (skipMode > 0) {
			char type = identifyLineType();
			if (type == TYPE_UNCONDITIONAL) {
				skipMode--;
			} else if (type == TYPE_CONDITIONAL) {
				skipMode++;
			}
			err = 0;
		} else {
			err = evalLine(pos, pos + size);
		}
		//Not an actual error.
		if (err == ERROR_CONDITIONAL_UNTRIGGERED)
			err = 0;
		if (err == ERROR_HALTING)
			break;
		if (err != 0) {
			//This error means something occurred
			//	that requires changing the address.
			if (err == ERROR_CHANGE_ADDRESS) {
				pos = (ibword)evaluateFormula();
				eof = 10;
				continue;
			} else if (err == ERROR_CONDITIONAL_TRIGGERED) {
				skipMode = 1;
			} else {
				printError(err);
				printString(" at position ");
				printInt(pos);
				printString(".\n");
				return err;
			}
		}
		pos += size;
	}
	
	if (skipMode != 0) {
		printString("Unclosed IF statement.\n");
		return 50;
	}
	return 0;
}
