#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../hardware/hardware.h"
#include "journal.c"

#define EVAL_NUM_BUFF_SIZE 13
#define EVAL_BUFF_SIZE 50
char EVAL_BUFF[EVAL_BUFF_SIZE];
char EVAL_NUM_BUFF[EVAL_NUM_BUFF_SIZE];
char EVAL_LEN;

void evaluateFormulaOnce() {
	char pos = -1;
	short max_precedence = -1;
	short relative_precedence = 0;
	short absolute_precedence = 0;
	float LHS = 0;
	float RHS = 0;
	float output = 0;
	char RHS_pos = -1;
	char LHS_pos = -1;
	char decimal = 0;
	char elen = 0;
	char oper;
	
	for (char i = 0; i < EVAL_LEN; i++) {
		char c = EVAL_BUFF[i];
		if (c == '=' || c == '~')
			absolute_precedence = 1;
		else if (c == '+' || c == '-')
			absolute_precedence = 2;
		else if (c == '*' || c == '/')
			absolute_precedence = 3;
		else if (isOperator(c))
			absolute_precedence = 4;
		else if (c == '(')
			relative_precedence += 10;
		else if (c == ')')
			relative_precedence -= 10;
		
		if (isOperator(c)) {
			short precedence = relative_precedence + absolute_precedence;
			if (precedence > max_precedence) {
				max_precedence = precedence;
				pos = i;
			}
		}
	}
	oper = EVAL_BUFF[pos];
	
	//Get RHS.
	if (EVAL_BUFF[pos + 1] != '[') {
		decimal = 0;
		for (char i = pos + 1; i < EVAL_LEN; i++) {
			RHS_pos = i;
			char c = EVAL_BUFF[i];
			if (isOperator(c) || c == '(' || c == ')') {
				break;
			} else if (c >= '0' && c <= '9') {
				RHS *= 10;
				RHS += c - '0';
				if (decimal > 0) {
					decimal++;
				}
			} else if (c == '.') {
				decimal = 1;
			}
		}
		for (char i = 0; i < decimal - 1; i++) {
			RHS /= 10;
		}
	} else {
		for (char i = pos + 1; i < EVAL_LEN; i++) {
			RHS_pos = i;
			char c = EVAL_BUFF[i];
			if (isOperator(c) || c == '(' || c == ')') {
				break;
			} else if (c >= '0' && c <= '9') {
				RHS *= 10;
				RHS += c - '0';
			}
		}
		RHS = journal_pop(RHS);
	}
	RHS_pos--;
	
	//Get LHS.
	decimal = 0;
	char enb_pos = EVAL_NUM_BUFF_SIZE - 2;
	if (EVAL_BUFF[pos - 1] != ']') {
		for (char i = pos - 1; i != 255; i--) {
			LHS_pos = i;
			char c = EVAL_BUFF[i];
			if (isOperator(c) || c == '(' || c == ')') {
				break;
			} else if ( (c >= '0' && c <= '9') || c == '.' ) {
				EVAL_NUM_BUFF[enb_pos--] = c;
			}
		}
		while (enb_pos > 0) {
			EVAL_NUM_BUFF[enb_pos] = ' ';
			enb_pos--;
		}
		EVAL_NUM_BUFF[enb_pos] = ' ';
		EVAL_NUM_BUFF[EVAL_NUM_BUFF_SIZE - 1] = 0;
		LHS = atof(EVAL_NUM_BUFF);
	} else {
		for (char i = pos - 1; i != 255; i--) {
			LHS_pos = i;
			char c = EVAL_BUFF[i];
			if (isOperator(c) || c == '(' || c == ')') {
				break;
			} else if ( (c >= '0' && c <= '9') || c == '.' ) {
				EVAL_NUM_BUFF[enb_pos--] = c;
			}
		}
		while (enb_pos > 0) {
			EVAL_NUM_BUFF[enb_pos] = ' ';
			enb_pos--;
		}
		EVAL_NUM_BUFF[enb_pos] = ' ';
		EVAL_NUM_BUFF[EVAL_NUM_BUFF_SIZE - 1] = 0;
		LHS = atof(EVAL_NUM_BUFF);
		LHS = journal_pop(LHS);
	}
	LHS_pos++;
	
	//Remove brackets.
	if (LHS_pos != 0 && RHS_pos != EVAL_LEN - 1) {
		if (EVAL_BUFF[LHS_pos - 1] == '(' && EVAL_BUFF[RHS_pos + 1] == ')') {
			LHS_pos--;
			RHS_pos++;
		}
		if (LHS_pos == 1 && RHS_pos == EVAL_LEN - 2) {
			LHS_pos = 0;
			RHS_pos = EVAL_LEN - 1;
		}
	}
	
	if (oper == '+') {
		output = LHS + RHS;
	} else if (oper == '-') {
		output = LHS - RHS;
	} else if (oper == '*') {
		output = LHS * RHS;
	} else if (oper == '/') {
		output = LHS / RHS;
	} else if (oper == '=') {
		output = LHS == RHS;
	} else if (oper == '>') {
		output = (int)LHS >> (int)RHS;
	} else if (oper == '<') {
		output = (int)LHS << (int)RHS;
	} else if (oper == '%') {
		output = (int)LHS % (int)RHS;
	} else if (oper == '|') {
		output = (int)LHS | (int)RHS;
	} else if (oper == '&') {
		output = (int)LHS & (int)RHS;
	} else if (oper == '^') {
		output = (int)LHS ^ (int)RHS;
	} else if (oper == '~') {
		output = LHS != RHS;
	}
	
	
	for (char i = 0; i <= LHS_pos - 1; i++) {
		EVAL_BUFF[elen++] = EVAL_BUFF[i];
	}
	
	decimal = journal_push(output);
	EVAL_BUFF[elen++] = '[';
	EVAL_BUFF[elen++] = (decimal + '0');
	EVAL_BUFF[elen++] = ']';
	
	for (char i = RHS_pos + 1; i < EVAL_LEN; i++) {
		EVAL_BUFF[elen++] = EVAL_BUFF[i];
	}
	
	EVAL_LEN = elen;
	
	EVAL_BUFF[elen] = 0;
	

}

//Evaluates a string stored in EVAL_BUFF.
float evaluateFormula() {
	
	journal_init();
	while (EVAL_LEN != 3) {
		evaluateFormulaOnce();
	}
	
	return journal_pop(0);
}

//Loads a formula from program memory into the EVAL_BUFF.
bool loadFormula(word pos) {
	//012345678
	//5+(5+0*2)

}