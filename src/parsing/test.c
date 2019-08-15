#include <stdio.h>
#include "formula_dynamic.c"
int main2() {
	Evaluator e;
	e.len = 5;
	char str[] = "2+2*2";
	for (unsigned char i = 0; i < 6; i++)
		e.buff[i] = str[i];

	printf("%f\n", evaluateFormula(&e));


	return 0;
}

int main() {
	return main2();
}
