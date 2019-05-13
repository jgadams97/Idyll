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
char copyFormulaIntoEvalBuff(ibword pos, ibword size) {

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
		if (isWS(LINE_BUFF[pos + i])) continue;
		pc = c;
		c = LINE_BUFF[pos + i];
		if ((isAlpha(c) || c == '@') && !varMode) { 
			varMode = true;
			varPos = 0;
			key[varPos++] = c;
		} else if (isAlphaNum(c) && varMode) {
			key[varPos++] = c;
		} else if (!isAlphaNum(varMode) && varMode) {
			key[varPos] = 0;
			ibword addr = findNode(key);
			if (addr == (ibword)undefined) return ERROR_KEY_NOT_FOUND;
			if (key[0] == '@') appendAdr(readAdr(addr));
			else if (LINE_BUFF[pos + i] == '[') {
				signed short index;
				i += 1;
				char numBuffPos = 0;
				char numIsVar = 0;
				char isOnlyNum = 1;
				char numBuff[20];
				char cc = ' ';
				for (; cc != ']'; i++) {
					if (i == size) return 0;
					if (isAlpha(cc))
						numIsVar = 1;
					if (!isNum(cc) && !isWS(cc))
						isOnlyNum = 0;
					if (!isWS(cc))
						numBuff[numBuffPos++] = cc;
					cc = LINE_BUFF[pos + i];
				}
				if (numBuffPos == 0)
					return ERROR_SYNTAX;
				numBuff[numBuffPos] = 0;
				if (numIsVar) {
					if (!verifyKey(numBuff))
						return ERROR_INVALID_KEY;
					ibword addrIndex = findNode(numBuff);
					if (addrIndex == (ibword)undefined)
						return ERROR_KEY_NOT_FOUND;
					index = (int)readNum(addrIndex);
				} else {
					if (isOnlyNum)
						index = atoi(numBuff);
					else
						return ERROR_SYNTAX;
				}
				index--;
				if (index < 0 || index >= readArrSize(addr))
					return ERROR_OUT_OF_BOUNDS;
					
				appendNum(readArr(addr, index));
					
			} else {
				appendNum(readNum(addr));
			}
			
				
			varMode = false;
		}
			
		//Removes excessive parenthesis.
		if (c == '(') {
			fixerStack[fixerStackPos++] = EVAL_LEN;
		} else if (c == ')') {
			ibword fs = fixerStack[fixerStackPos - 1];
			fixerStackPos--;
			if (fs != (ibword)-1) {
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
	if (varMode) {
		key[varPos] = 0;
		ibword addr = findNode(key);
		if (addr == (ibword)undefined) return ERROR_KEY_NOT_FOUND;
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
	
	//Fix negatives.
	EVAL_LEN = 0;
	pc = 0;
	c = ' ';
	for (char i = 0; i < size; i++) {
		pc = c;
		c = EVAL_BUFF[i];
		if (c == '-') {
			if (pc == 0 || isOperator(pc) || pc == '(') {
				TMP_EVAL_BUFF[EVAL_LEN++] = '(';
				TMP_EVAL_BUFF[EVAL_LEN++] = '0';
				TMP_EVAL_BUFF[EVAL_LEN++] = '-';
				TMP_EVAL_BUFF[EVAL_LEN++] = '1';
				TMP_EVAL_BUFF[EVAL_LEN++] = ')';
				TMP_EVAL_BUFF[EVAL_LEN++] = '*';
			} else {
				TMP_EVAL_BUFF[EVAL_LEN++] = c;				
			}
		} else {
			TMP_EVAL_BUFF[EVAL_LEN++] = c;
		}
	}
	TMP_EVAL_BUFF[EVAL_LEN] = 0;
	unimemcpy(EVAL_BUFF, TMP_EVAL_BUFF, EVAL_LEN + 1);
	
	return 0;
}

//Copies a string or string formula to EVAL_BUFF for string processing.
//	Returns false if a variable didn't exist.
char copyStringIntoEvalBuff(ibword pos, ibword size) {
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
					if (addr == (ibword)undefined) return ERROR_KEY_NOT_FOUND;
					if (key[0] == '$' && c != '[') {
						EVAL_BUFF[EVAL_LEN++] = '#';
						appendAdr(addr);
						EVAL_BUFF[EVAL_LEN++] = ':';
						appendAdr(readStrSize(addr));
					} else if (key[0] == '@') {
						appendAdr(readAdr(addr));
					} else if (LINE_BUFF[pos + i] == '[') {
						ibword index;
						i += 1;
						char numBuffPos = 0;
						char numIsVar = 0;
						char isOnlyNum = 1;
						char numBuff[20];
						char cc = ' ';
						for (; cc != ']'; i++) {
							if (i == size) return 0;
							if (isAlpha(cc))
								numIsVar = 1;
							if (!isNum(cc) && !isWS(cc))
								isOnlyNum = 0;
							if (!isWS(cc))
								numBuff[numBuffPos++] = cc;
							cc = LINE_BUFF[pos + i];
						}
						if (numBuffPos == 0)
							return ERROR_SYNTAX;
						numBuff[numBuffPos] = 0;
						if (numIsVar) {
							if (!verifyKey(numBuff))
								return ERROR_INVALID_KEY;
							ibword addrIndex = findNode(numBuff);
							if (addrIndex == (ibword)undefined)
								return ERROR_KEY_NOT_FOUND;
							index = (int)readNum(addrIndex);
						} else {
							if (isOnlyNum)
								index = atoi(numBuff);
							else
								return ERROR_SYNTAX;
						}
						index--;
						if (key[0] == '$') {
							if (index < 0 || index >= readStrSize(addr))
								return ERROR_OUT_OF_BOUNDS;
							EVAL_BUFF[EVAL_LEN++] = readStr(addr, index);
						} else {
							if (index < 0 || index >= readArrSize(addr))
								return ERROR_OUT_OF_BOUNDS;
							appendNum(readArr(addr, index));
						}
						varMode = false;
						continue;
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
		if (addr == (ibword)undefined) return ERROR_KEY_NOT_FOUND;
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
	return 0;
}

//Verifies no syntax errors in formulas.
bool verifyFormula(ibword pos, ibword size) {
	char balance = 0;
	char c = 0;
	char pc;
	char type = 'x';
	char ptype = ' ';
	char ps = ' ';
	for (ibword i = 0; i < size; i++) {
		pc = c;
		c = LINE_BUFF[pos + i];
		if (isWS(c)) {
			ps = type;
			continue;
		}
		if (c == '(') {
			balance++;
			ptype = type;
			type = '(';
		} else if (c == ')') {
			balance--;
			ptype = type;
			type = ')';
		} else if (!isAlphaNum(c) && !isOperator(c) && !isWS(c) && c != '@') {
			if (c == '[') {
				if (!isAlphaNum(pc)) return false;
				char isVar = 1;
				char isNum = 1;
				i++;
				for (; c != ']'; i++) {
					if (i == size)
						return false;
					c = LINE_BUFF[pos + i];
					if (!isAlphaNum(c) && !isWS(c) && c != ']')
						return false;
				}
				continue;
			}
			return false;
		}
		if (isOperator(c)) {
			ptype = type;
			if (c == '-') type = '-';
			else type = '+';
		} else if (isAlphaNum(c) || c == '@') {
			ptype = type;
			if (c == '.') type = '.';
			else type = '0';
		}
		
		if (type == '+') {
			if (ptype == '(' || ptype == 'x' || ptype == '+') {
				return false;
			}
		} else if (type == '(') {
			if (ptype == ')' || ptype == '0' || ptype == '.') {
				return false;
			}
		} else if (type == ')') {
			if (ptype == '(' || ptype == '+') {
				return false;
			}
		} else if (type == '0') {
			if (ptype == ')') {
				return false;
			}
			if (ptype == '.' && !isNum(c)) {
				return false;
			}
			if (ps == '0' || ps == '.') {
				return false;
			}
		} else if (type == '.') {
			if (ptype == '.' || ptype == ')') {
				return false;
			}
		}
		ps = ' ';
	}
	
	if (type == '+' || type == '(' || type == '-') {
		return false;
	}
	
	if (balance != 0)
		return false;
	return true;
}

//Verifies no syntax errors in string formulas.
bool verifyString(ibword pos, ibword size) {
	bool stringMode = false;
	char c = 'x';
	char pc;
	
	for (ibword i = 0; i < size; i++) {
		if (isWS(LINE_BUFF[pos + i])) continue;
		pc = c;
		c = LINE_BUFF[pos + i];
		if (!stringMode) {
			if (c == '\"') {
				stringMode = true;
			} else if (c == '+' && pc == '+') {
				return false;
			} else if (!isAlphaNum(c) && c != '$' && c != '@' && c != '+') {
				if (c == '[') {
					if (!isAlphaNum(pc)) return false;
					char isVar = 1;
					char isNum = 1;
					i++;
					for (; c != ']'; i++) {
						if (i == size)
							return false;
						c = LINE_BUFF[pos + i];
						if (!isAlphaNum(c) && !isWS(c) && c != ']')
							return false;
					}
					continue;
				}
				return false;
			}
		} else if (stringMode) {
			if (c == '\"') {
				stringMode = false;
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
	
	if (EVAL_ADR == (ibword)undefined) {
		if (EVAL_POS >= EVAL_LEN) {
			EVAL_POS = 0;
			EVAL_ADR = undefined;
			EVAL_STR_SIZE = 0;
			EVAL_STR = false;
			EVAL_STR_POS = 0;
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
			c = readStr(EVAL_ADR, EVAL_STR_POS++);
			if (c == 0) {
				EVAL_ADR = undefined;
				return readCharFromEvalBuff();
			}
			EVAL_STR_SIZE--;
		} else {
			EVAL_ADR = undefined;
			EVAL_STR_POS = 0;
			return readCharFromEvalBuff();
		}
	}
	return c;

}
