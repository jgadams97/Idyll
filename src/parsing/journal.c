#define JOURNAL_MAX 8
char JOURNAL_DATA_SIZE;
char JOURNAL_LOG_SIZE;
char JOURNAL_LOG[JOURNAL_MAX];
float JOURNAL_DATA[JOURNAL_MAX];

void journal_print() {
	printf("Log: ");
	for (char i = 0; i < JOURNAL_LOG_SIZE; i++) {
		printf("%i", JOURNAL_LOG[i]);
		if (i != JOURNAL_LOG_SIZE - 1) {
			printf(",");
		}
	}
	printf("\n");
	printf("Data: ");
	for (char i = 0; i < JOURNAL_DATA_SIZE; i++) {
		printf("%f", JOURNAL_DATA[i]);
		if (i != JOURNAL_DATA_SIZE - 1) {
			printf(",");
		}
	}
	printf("\n");
}
//Initialize the journal.
void journal_init() {
	JOURNAL_LOG_SIZE = 0;
	JOURNAL_DATA_SIZE = 0;
}

//Push to journal.
//	Returns location pushed.
char journal_push(float a) {
	if (JOURNAL_LOG_SIZE == 0) {
		if (JOURNAL_DATA_SIZE == JOURNAL_MAX) {
			return -1;
		} else  {
			JOURNAL_DATA[JOURNAL_DATA_SIZE++] = a;
			return JOURNAL_DATA_SIZE - 1;
		}
	} else {
		JOURNAL_DATA[JOURNAL_LOG[JOURNAL_LOG_SIZE - 1]] = a;
		JOURNAL_LOG_SIZE--;
		return JOURNAL_LOG[JOURNAL_LOG_SIZE];
	}
}

//Peek at journal.
float journal_peek(char i) {
	if (i >= JOURNAL_MAX) return -1;
	return JOURNAL_DATA[i];
}

//Pops data off of journal.
float journal_pop(char i) {
	if (i < 0 || i >= JOURNAL_DATA_SIZE) return -1;
	if (i == JOURNAL_DATA_SIZE - 1) {
		JOURNAL_DATA_SIZE--;
		return JOURNAL_DATA[JOURNAL_DATA_SIZE];
	} else {
		JOURNAL_LOG[JOURNAL_LOG_SIZE++] = i;
		return JOURNAL_DATA[i];
	}
}



/*

	
	Domains
		0 = 1:7
		1 = 0:12
		2 = 16:22
		3 = 0:22
	
	To determine if a side is a constant or
	a variable, see if the domain of that
	side is the same as the domain of another.
	
	Constant + Constant requires pushing a new
	journal entry to store the results.
	Constant + Variable or Variable + Constant
	does not as we can simply store the
	results inside of the variable. Variable +
	Variable requires us to pop a variable
	from the journal because the results
	combine the two, and only one is needed to
	store the results, so the other can be
	discarded.
 	
	
	0
		LHS = Constant 2
		RHS = Constant 2
		Operator = 4
		v0 = 4
	1
		LHS = v0
		RHS = 2
		Operator = *
		v0 = 8
	2
		
	



*/
