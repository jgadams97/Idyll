//Gets the type of expression (either a number or a string)
char getExprType(Evaluator *e, ibword pos, ibword size) {
	for (char i = 0; i < size; i++) {
		char c = e->line_buff[i + pos];
		if (c == '$' || c == '\"') {
			return TYPE_STR;
		}
	}
	return TYPE_NUM;
}


//Appends a floating poibword number to the end of e->buff.
void appendNum(Evaluator *e, float num) {
	char buff[20];
	floatToString(buff, num);
	for (char i = 0; i < strlen(buff); i++) {
		e->buff[e->len++] = buff[i];
	}
}

//Appends an address to the end of e->buff.
void appendAdr(Evaluator *e, ibword num) {
	char buff[20];
	ibwordToString(buff, num);
	for (char i = 0; i < strlen(buff); i++) {
		e->buff[e->len++] = buff[i];
	}
}

/*5+-3
-3
-(5+4)*/

//


//Copies a numeric formula to e->buff for numeric processing.
char copyFormulaIntoEvalBuff(Evaluator *e, ibword pos, ibword size) {

	short fixerStack[20];
	char fixerStackPos = 0;
	char key[KEY_SIZE + 1];
	char varPos = 0;
	char c = 0;
	char pc = 0;
	bool varMode = false;
	e->len = 0;
	e->buff[e->len++] = '(';
	for (char i = 0; i < size; i++) {
		if (isWS(e->line_buff[pos + i])) continue;
		pc = c;
		c = e->line_buff[pos + i];
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
			if (key[0] == '@') appendAdr(e, readAdr(addr));
			else if (e->line_buff[pos + i] == '[') {
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
					cc = e->line_buff[pos + i];
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
					
				appendNum(e, readArr(addr, index));
					
			} else {
				appendNum(e, readNum(addr));
			}
			
				
			varMode = false;
		}
			
		//Removes excessive parenthesis.
		if (c == '(') {
			fixerStack[fixerStackPos++] = e->len;
		} else if (c == ')') {
			ibword fs = fixerStack[fixerStackPos - 1];
			fixerStackPos--;
			if (fs != (ibword)-1) {
				e->buff[e->len++] = '+';
				e->buff[e->len++] = '0';
			}
		} else if (isOperator(c)) {
			fixerStack[fixerStackPos - 1] = -1;
		}
		
		if (!varMode) {
			if (c == '-') {
				if (pc == 0 || isOperator(pc) || pc == '(') {
					e->buff[e->len++] = '(';
					e->buff[e->len++] = '0';
					e->buff[e->len++] = '-';
					e->buff[e->len++] = '1';
					e->buff[e->len++] = ')';
					e->buff[e->len++] = '*';
				} else {
					e->buff[e->len++] = c;				
				}
			} else {
				e->buff[e->len++] = c;
			}
				
		}
		
	}
	if (varMode) {
		key[varPos] = 0;
		ibword addr = findNode(key);
		if (addr == (ibword)undefined) return ERROR_KEY_NOT_FOUND;
		if (key[0] == '@') appendAdr(e, readAdr(addr));
		else appendNum(e, readNum(addr));
	}
	
	//Need +0 in case no ibwordernal operator.
	e->buff[e->len++] = '+';
	e->buff[e->len++] = '0';
	e->buff[e->len++] = ')';
	
	
	//Remove underscores added.
	size = e->len;
	e->len = 0;
	for (ibword i = 0; i < size; i++) {
		c = e->buff[i];
		if (c != '_') {
			e->buff[e->len++] = c;
		}
	}
	e->buff[e->len] = 0;
	
	//Fix negatives.
	e->len = 0;
	pc = 0;
	c = ' ';
	for (char i = 0; i < size; i++) {
		pc = c;
		c = e->buff[i];
		if (c == '-') {
			if (pc == 0 || isOperator(pc) || pc == '(') {
				e->tmp_buff[e->len++] = '(';
				e->tmp_buff[e->len++] = '0';
				e->tmp_buff[e->len++] = '-';
				e->tmp_buff[e->len++] = '1';
				e->tmp_buff[e->len++] = ')';
				e->tmp_buff[e->len++] = '*';
			} else {
				e->tmp_buff[e->len++] = c;				
			}
		} else {
			e->tmp_buff[e->len++] = c;
		}
	}
	e->tmp_buff[e->len] = 0;
	unimemcpy(e->buff, e->tmp_buff, e->len + 1);
	
	return 0;
}

//Copies a string or string formula to e->buff for string processing.
//	Returns false if a variable didn't exist.
char copyStringIntoEvalBuff(Evaluator *e, ibword pos, ibword size) {
	char key[KEY_SIZE + 1];
	char varPos = 0;
	bool varMode = false;
	bool stringMode = false;
	e->len = 0;
	e->pos = 0;
	e->addr = undefined;
	for (char i = 0; i < size; i++) {
		char c = e->line_buff[pos + i];
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
						e->buff[e->len++] = '#';
						appendAdr(e, addr);
						e->buff[e->len++] = ':';
						appendAdr(e, readStrSize(addr));
					} else if (key[0] == '@') {
						appendAdr(e, readAdr(addr));
					} else if (e->line_buff[pos + i] == '[') {
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
							cc = e->line_buff[pos + i];
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
							e->buff[e->len++] = readStr(addr, index);
						} else {
							if (index < 0 || index >= readArrSize(addr))
								return ERROR_OUT_OF_BOUNDS;
							appendNum(e, readArr(addr, index));
						}
						varMode = false;
						continue;
					} else {
						appendNum(e, readNum(addr));
					}
			
					varMode = false;
				}
			}
			
			if (!varMode) {
				e->buff[e->len++] = c;
			}
		}
	}
	if (varMode) {
		key[varPos] = 0;
		ibword addr = findNode(key);
		if (addr == (ibword)undefined) return ERROR_KEY_NOT_FOUND;
		if (key[0] == '$') {
			e->buff[e->len++] = '#';
			appendAdr(e, addr);
			e->buff[e->len++] = ':';
			appendAdr(e, readStrSize(addr));
		} else if (key[0] == '@') {
			appendAdr(e, readAdr(addr));
		} else {
			appendNum(e, readNum(addr));
		}
	}
	e->buff[e->len] = 0;
	return 0;
}

//Verifies no syntax errors in formulas.
bool verifyFormula(Evaluator *e, ibword pos, ibword size) {
	char balance = 0;
	char c = 0;
	char pc;
	char type = 'x';
	char ptype = ' ';
	char ps = ' ';
	for (ibword i = 0; i < size; i++) {
		pc = c;
		c = e->line_buff[pos + i];
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
					c = e->line_buff[pos + i];
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
bool verifyString(Evaluator *e, ibword pos, ibword size) {
	bool stringMode = false;
	char c = 'x';
	char pc;
	
	for (ibword i = 0; i < size; i++) {
		if (isWS(e->line_buff[pos + i])) continue;
		pc = c;
		c = e->line_buff[pos + i];
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
						c = e->line_buff[pos + i];
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

//If e->buff contains a string formula, use this to read
//	a character from it. Characters are shifted out of it.
//	The last character will be a 0.
char readCharFromEvalBuff(Evaluator *e) {
	char c;
	
	if (e->addr == (ibword)undefined) {
		if (e->pos >= e->len) {
			e->pos = 0;
			e->addr = undefined;
			e->str_size = 0;
			e->str = false;
			e->str_pos = 0;
			return 0;
		}
		c = e->buff[e->pos++];
	
		while (c == '\"' || (c == '+' && !e->str)) {
			if (e->pos >= e->len) {
				e->pos = 0;
				e->addr = undefined;
				e->str_size = 0;
				e->str = false;
				return 0;
			}
			if (c == '\"') e->str = !e->str;
			c = e->buff[e->pos++];
			
			
		}
		//If we encountered a string variable...
		if (c == '#' && !e->str) {
			char numBuff[20];
			//Read address.
			char numBuffPos = 0;
			c = e->buff[e->pos++];
			while (isNum(c)) {
				numBuff[numBuffPos++] = c;
				c = e->buff[e->pos++];
			}
			numBuff[numBuffPos] = 0;
			e->addr = atoi(numBuff);
			//Read size.
			numBuffPos = 0;
			c = e->buff[e->pos++];
			while (isNum(c)) {
				numBuff[numBuffPos++] = c;
				c = e->buff[e->pos++];
				if (e->pos == e->len + 1)
					break;
			}
			numBuff[numBuffPos] = 0;
			e->str_size = atoi(numBuff);
			return readCharFromEvalBuff(e);
		}
	} else {
		if (e->str_size > 0) {
			c = readStr(e->addr, e->str_pos++);
			if (c == 0) {
				e->addr = undefined;
				return readCharFromEvalBuff(e);
			}
			e->str_size--;
		} else {
			e->addr = undefined;
			e->str_pos = 0;
			return readCharFromEvalBuff(e);
		}
	}
	return c;

}
