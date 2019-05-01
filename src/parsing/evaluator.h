#include "../variables/variables.c"
#include "formula.c"

//Error types.
#define ERROR_SYNTAX 1
#define ERROR_KEY_NOT_FOUND 2
#define ERROR_INVALID_KEY 3
#define ERROR_KEY_EXISTS 4
#define ERROR_OUT_OF_BOUNDS 5
#define ERROR_INVALID_TYPE 6
#define ERROR_ARGUMENT_COUNT 7
#define ERROR_CHANGE_ADDRESS 8
#define ERROR_CONDITIONAL_TRIGGERED 9
#define ERROR_UNCONDITIONAL_TRIGGERED 10
#define ERROR_INVALID_COMPARISON 11
#define ERROR_MISSING_THEN 12
#define ERROR_STRING_TOO_LARGE 13
#define ERROR_INVALID_OPTION 14
#define ERROR_UNKNOWN_COMMAND 15
#define ERROR_FILE_NOT_FOUND 16

//Variable types.
#define TYPE_NUM 0
#define TYPE_STR 1
//Line types.
#define TYPE_COMMAND 0
#define TYPE_DECLARATION 1
#define TYPE_ASSIGNMENT 2
#define TYPE_REFERENCE 3
#define TYPE_CONDITIONAL 4
#define TYPE_UNCONDITIONAL 5
#define TYPE_COMMENT 6
#define TYPE_EMPTY 7
#define LINE_BUFF_MAX 100
#define STRING_SIZE_MAX 1000
word EVAL_POS = 0;
word EVAL_ADR = undefined;
word EVAL_STR_SIZE = 0;
bool EVAL_STR = false;
char LINE_BUFF[LINE_BUFF_MAX + 1];

//Gets the type of expression (either a number or a string)
char getExprType(word pos, word size);

//Appends a floating point number to the end of EVAL_BUFF.
void appendNum(float num);

//Appends an address to the end of EVAL_BUFF.
void appendAdr(word num);

//Copies a numeric formula to EVAL_BUFF for numeric processing.
//	Returns false if a variable didn't exist.
bool copyFormulaIntoEvalBuff();

//Copies a string or string formula to EVAL_BUFF for string processing.
//	Returns false if a variable didn't exist.
bool copyStringIntoEvalBuff();

//If EVAL_BUFF contains a string formula, use this to read
//	a character from it. Characters are shifted out of it.
//	The last character will be a 0.
char readCharFromEvalBuff();

#include "evaluator.c"