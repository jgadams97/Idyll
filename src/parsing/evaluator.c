//Gets the type of expression (either a number or a string)
char getExprType(ibword pos, ibword size) {
	for (char i = 0; i < size; i++) {
		char c = LINE_BUFF[i + pos];
		if (c == '$' || c == '\"') {
			return TYPE_STR;
		}
	}
	return TYPE_NUM;
}


//Appends a floating poibword number to the end of EVAL_BUFF.
void appendNum(float num) {
	char buff[20];
	if ( (ibword)num == num )
		ibwordToString(buff, (ibword)num);
	else
		floatToString(buff, num);
	
	for (char i = 0; i < strlen(buff); i++) {
		EVAL_BUFF[EVAL_LEN++] = buff[i];
	}
}

//Appends an address to the end of EVAL_BUFF.
void appendAdr(ibword num) {
	char buff[20];
	ibwordToString(buff, num);
	for (char i = 0; i < strlen(buff); i++) {
		EVAL_BUFF[EVAL_LEN++] = buff[i];
	}
}

/*5+-3
-3
-(5+4)*/

//


//Copies a numeric formula to EVAL_BUFF for numeric processing.
//	0 No Error.
bool copyFormulaIntoEvalBuff(ibword pos, ibword size) {

	short fixerStack[20];
	char fixerStackPos = 0;
	char key[KEY_SIZE + 1];
	char varPos = 0;
	char c = 0;
	char pc = 0;
	bool varMode = false;
	EVAL_LEN = 0;
	EVAL_BUFF[EVAL_LEN++] = '(';
	for (char i = 0; i < size; i++) {
		pc = c;
		char c = LINE_BUFF[pos + i];
		if (!isWS(c)) {
		
			if ((isAlpha(c) || c == '@') && !varMode) { 
				varMode = true;
				varPos = 0;
				key[varPos++] = c;
			} else if (isAlphaNum(c) && varMode) {
				key[varPos++] = c;
			} else if (!isAlphaNum(varMode) && varMode) {
				key[varPos] = 0;
				ibword addr = findNode(key);
				if (addr == undefined) return false;
				if (key[0] == '@') appendAdr(readAdr(addr));
				else appendNum(readNum(addr));
				varMode = false;
			}
			
			//Removes excessive parenthesis.
			if (c == '(') {
				fixerStack[fixerStackPos++] = EVAL_LEN;
			} else if (c == ')') {
				ibword fs = fixerStack[fixerStackPos - 1];
				fixerStackPos--;
				if (fs != -1) {
					EVAL_BUFF[EVAL_LEN++] = '+';
					EVAL_BUFF[EVAL_LEN++] = '0';
				}
			} else if (isOperator(c)) {
				fixerStack[fixerStackPos - 1] = -1;
			}
			
			if (!varMode) {
				if (c == '-') {
					if (pc == 0 || isOperator(pc) || pc == '(') {
						EVAL_BUFF[EVAL_LEN++] = '(';
						EVAL_BUFF[EVAL_LEN++] = '0';
						EVAL_BUFF[EVAL_LEN++] = '-';
						EVAL_BUFF[EVAL_LEN++] = '1';
						EVAL_BUFF[EVAL_LEN++] = ')';
						EVAL_BUFF[EVAL_LEN++] = '*';
					} else {
						EVAL_BUFF[EVAL_LEN++] = c;				
					}
				} else {
					EVAL_BUFF[EVAL_LEN++] = c;
				}
				
			}
			
			
		}
	}
	if (varMode) {
		key[varPos] = 0;
		ibword addr = findNode(key);
		if (addr == undefined) return false;
		if (key[0] == '@') appendAdr(readAdr(addr));
		else appendNum(readNum(addr));
	}
	
	//Need +0 in case no ibwordernal operator.
	EVAL_BUFF[EVAL_LEN++] = '+';
	EVAL_BUFF[EVAL_LEN++] = '0';
	EVAL_BUFF[EVAL_LEN++] = ')';
	
	
	//Remove underscores added.
	size = EVAL_LEN;
	EVAL_LEN = 0;
	for (ibword i = 0; i < size; i++) {
		c = EVAL_BUFF[i];
		if (c != '_') {
			EVAL_BUFF[EVAL_LEN++] = c;
		}
	}
	EVAL_BUFF[EVAL_LEN] = 0;
	
	return true;
}

//Copies a string or string formula to EVAL_BUFF for string processing.
//	Returns false if a variable didn't exist.
bool copyStringIntoEvalBuff(ibword pos, ibword size) {
	char key[KEY_SIZE + 1];
	char varPos = 0;
	bool varMode = false;
	bool stringMode = false;
	EVAL_LEN = 0;
	EVAL_POS = 0;
	EVAL_ADR = undefined;
	for (char i = 0; i < size; i++) {
		char c = LINE_BUFF[pos + i];
		if (!isWS(c) || stringMode) {
			if (c == '\"') stringMode = !stringMode;
			if (!stringMode) {
				if ((isAlpha(c) || c == '@' || c == '$') && !varMode) { 
					varMode = true;
					varPos = 0;
					key[varPos++] = c;
				} else if (isAlphaNum(c) && varMode) {
					key[varPos++] = c;
				} else if (!isAlphaNum(varMode) && varMode) {
					key[varPos] = 0;
					ibword addr = findNode(key);
					if (addr == undefined) return false;
					if (key[0] == '$') {
						EVAL_BUFF[EVAL_LEN++] = '#';
						appendAdr(addr);
						EVAL_BUFF[EVAL_LEN++] = ':';
						appendAdr(readStrSize(addr));
					} else if (key[0] == '@') {
						appendAdr(readAdr(addr));
					} else {
						appendNum(readNum(addr));
					}
					varMode = false;
				}
			}
			
			if (!varMode) {
				EVAL_BUFF[EVAL_LEN++] = c;
			}
		}
	}
	if (varMode) {
		key[varPos] = 0;
		ibword addr = findNode(key);
		if (addr == undefined) return false;
		if (key[0] == '$') {
			EVAL_BUFF[EVAL_LEN++] = '#';
			appendAdr(addr);
			EVAL_BUFF[EVAL_LEN++] = ':';
			appendAdr(readStrSize(addr));
		} else if (key[0] == '@') {
			appendAdr(readAdr(addr));
		} else {
			appendNum(readNum(addr));
		}
	}
	EVAL_BUFF[EVAL_LEN] = 0;
	return true;
}

//Verifies no syntax errors in formulas.
bool verifyFormula(ibword pos, ibword size) {
	char balance = 0;
	char c;
	for (ibword i = 0; i < size; i++) {
		c = LINE_BUFF[pos + i];
		if (c == '(')
			balance++;
		else if (c == ')')
			balance--;
		else if (!isAlphaNum(c) && !isOperator(c) && !isWS(c) && c != '@')
			return false;
	}
	if (balance != 0)
		return false;
	return true;
}

//Verifies no syntax errors in string formulas.
bool verifyString(ibword pos, ibword size) {
	return true;
	bool stringMode = false;
	char c;
	for (ibword i = 0; i < size; i++) {
		c = LINE_BUFF[pos];
		if (!stringMode) {
			if (c == '\"') {
				stringMode = true;
			} else {
				//if (!isAlphaNum(c) && 
			}
		}
	}
	if (stringMode)
		return false;
	return true;
}

//If EVAL_BUFF contains a string formula, use this to read
//	a character from it. Characters are shifted out of it.
//	The last character will be a 0.
char readCharFromEvalBuff() {
	char c;
	
	if (EVAL_ADR == undefined) {
		if (EVAL_POS >= EVAL_LEN) {
			EVAL_POS = 0;
			EVAL_ADR = undefined;
			EVAL_STR_SIZE = 0;
			EVAL_STR = false;
			return 0;
		}
		c = EVAL_BUFF[EVAL_POS++];
	
		while (c == '\"' || (c == '+' && !EVAL_STR)) {
			if (EVAL_POS >= EVAL_LEN) {
				EVAL_POS = 0;
				EVAL_ADR = undefined;
				EVAL_STR_SIZE = 0;
				EVAL_STR = false;
				return 0;
			}
			if (c == '\"') EVAL_STR = !EVAL_STR;
			c = EVAL_BUFF[EVAL_POS++];
			
			
		}
		//If we encountered a string variable...
		if (c == '#' && !EVAL_STR) {
			char numBuff[20];
			//Read address.
			char numBuffPos = 0;
			c = EVAL_BUFF[EVAL_POS++];
			while (isNum(c)) {
				numBuff[numBuffPos++] = c;
				c = EVAL_BUFF[EVAL_POS++];
			}
			numBuff[numBuffPos] = 0;
			EVAL_ADR = atoi(numBuff);
			//Read size.
			numBuffPos = 0;
			c = EVAL_BUFF[EVAL_POS++];
			while (isNum(c)) {
				numBuff[numBuffPos++] = c;
				c = EVAL_BUFF[EVAL_POS++];
				if (EVAL_POS == EVAL_LEN + 1)
					break;
			}
			numBuff[numBuffPos] = 0;
			EVAL_STR_SIZE = atoi(numBuff);
			return readCharFromEvalBuff();
		}
	} else {
		if (EVAL_STR_SIZE > 0) {
			c = readStr(EVAL_ADR++, 0);
			if (c == 0) {
				EVAL_ADR = undefined;
				return readCharFromEvalBuff();
			}
			EVAL_STR_SIZE--;
		} else {
			EVAL_ADR = undefined;
			return readCharFromEvalBuff();
		}
	}
	return c;

}