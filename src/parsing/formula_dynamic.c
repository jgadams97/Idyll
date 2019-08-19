#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../hardware/hardware.h"
#include "journal_dynamic.c"

#define NUM_BUFF_SIZE 13
#define BUFF_SIZE 50
#define LINE_BUFF_MAX 100
typedef struct {
	char tmp_buff[BUFF_SIZE];
	char buff[BUFF_SIZE];
	char num_buff[NUM_BUFF_SIZE];
	char in_ram;
	unsigned char len;
	ibword addr, load_addr;
 	signed short pos, str_size, str_pos;
	bool str;
	char line_buff[LINE_BUFF_MAX + 1];
	Journal j;
	char RAM[10000];
	ibword avl_root, avl_end, ram_size;
} Evaluator;

void evaluateFormulaOnce(Evaluator *e) {

	unsigned char pos = -1;
	short max_precedence = -1;
	short relative_precedence = 0;
	short absolute_precedence = 0;
	float LHS = 0;
	float RHS = 0;
	float output = 0;
	unsigned char RHS_pos = -1;
	unsigned char LHS_pos = -1;
	char decimal = 0;
	unsigned char elen = 0;
	char oper;

	for (unsigned char i = 0; i < e->len; i++) {
		char c = e->buff[i];
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
	oper = e->buff[pos];

	//Get RHS.
	if (e->buff[pos + 1] != '[') {
		decimal = 0;
		for (unsigned char i = pos + 1; i < e->len; i++) {
			RHS_pos = i;
			char c = e->buff[i];
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
		for (unsigned char i = pos + 1; i < e->len; i++) {
			RHS_pos = i;
			char c = e->buff[i];
			if (isOperator(c) || c == '(' || c == ')') {
				break;
			} else if (c >= '0' && c <= '9') {
				RHS *= 10;
				RHS += c - '0';
			}
		}
		RHS = journal_pop(&e->j, RHS);
	}
	RHS_pos--;

	//Get LHS.
	decimal = 0;
	unsigned char enb_pos = NUM_BUFF_SIZE - 2;
	if (e->buff[pos - 1] != ']') {
		for (unsigned char i = pos - 1; i != 255; i--) {
			LHS_pos = i;
			char c = e->buff[i];
			if (isOperator(c) || c == '(' || c == ')') {
				break;
			} else if ( (c >= '0' && c <= '9') || c == '.' ) {
				e->num_buff[enb_pos--] = c;
			}
		}
		while (enb_pos > 0) {
			e->num_buff[enb_pos] = ' ';
			enb_pos--;
		}
		e->num_buff[enb_pos] = ' ';
		e->num_buff[NUM_BUFF_SIZE - 1] = 0;
		LHS = atof(e->num_buff);
	} else {
		for (unsigned char i = pos - 1; i != 255; i--) {
			LHS_pos = i;
			char c = e->buff[i];
			if (isOperator(c) || c == '(' || c == ')') {
				break;
			} else if ( (c >= '0' && c <= '9') || c == '.' ) {
				e->num_buff[enb_pos--] = c;
			}
		}
		while (enb_pos > 0) {
			e->num_buff[enb_pos] = ' ';
			enb_pos--;
		}
		e->num_buff[enb_pos] = ' ';
		e->num_buff[NUM_BUFF_SIZE - 1] = 0;
		LHS = atof(e->num_buff);
		LHS = journal_pop(&e->j, LHS);
	}
	LHS_pos++;

	//Remove brackets.
	if (LHS_pos != 0 && RHS_pos != e->len - 1) {
		if (e->buff[LHS_pos - 1] == '(' && e->buff[RHS_pos + 1] == ')') {
			LHS_pos--;
			RHS_pos++;
		}
		if (LHS_pos == 1 && RHS_pos == e->len - 2) {
			LHS_pos = 0;
			RHS_pos = e->len - 1;
		}
	}

	if (oper == '+') {
		output = LHS + RHS;
	} else if (oper == '-') {
		output = LHS - RHS;
	} else if (oper == '*') {
		output = LHS * RHS;
	} else if (oper == '/') {
		if (RHS == 0) output = 0;
		else output = LHS / RHS;
	} else if (oper == '=') {
		output = LHS == RHS;
	} else if (oper == '>') {
		output = (ibword)LHS >> (ibword)RHS;
	} else if (oper == '<') {
		output = (ibword)LHS << (ibword)RHS;
	} else if (oper == '%') {
		output = (ibword)LHS % (ibword)RHS;
	} else if (oper == '|') {
		output = (ibword)LHS | (ibword)RHS;
	} else if (oper == '&') {
		output = (ibword)LHS & (ibword)RHS;
	} else if (oper == '^') {
		output = (ibword)LHS ^ (ibword)RHS;
	} else if (oper == '~') {
		output = LHS != RHS;
	}

	for (unsigned char i = 0; i <= LHS_pos - 1; i++) {
		e->buff[elen++] = e->buff[i];
	}

	decimal = journal_push(&e->j, output);
	e->buff[elen++] = '[';
	e->buff[elen++] = (decimal + '0');
	e->buff[elen++] = ']';

	for (unsigned char i = RHS_pos + 1; i < e->len; i++) {
		e->buff[elen++] = e->buff[i];
	}

	e->len = elen;

	e->buff[elen] = 0;


}

//Evaluates a string stored in e->buff.
float evaluateFormula(Evaluator *e) {

	journal_init(&e->j);
	while (e->len != 3) {
		evaluateFormulaOnce(e);
	}

	return journal_pop(&e->j, 0);
}
