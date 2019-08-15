#define JOURNAL_MAX 8
typedef struct {
	unsigned char data_size;
	unsigned char log_size;
	unsigned char log[JOURNAL_MAX];
	float data[JOURNAL_MAX];
} Journal;

//Initialize the journal.
void journal_init(Journal *j) {
	j->log_size = 0;
	j->data_size = 0;
}

//Push to journal.
//	Returns location pushed.
char journal_push(Journal *j, float a) {
	if (j->log_size == 0) {
		if (j->data_size == JOURNAL_MAX) {
			return -1;
		} else  {
			j->data[j->data_size++] = a;
			return j->data_size - 1;
		}
	} else {
		j->data[j->log[j->log_size - 1]] = a;
		j->log_size--;
		return j->log[j->log_size];
	}
}

//Peek at journal.
float journal_peek(Journal *j, unsigned char i) {
	if (i >= JOURNAL_MAX) return -1;
	return j->data[i];
}

//Pops data off of journal.
float journal_pop(Journal *j, unsigned char i) {
	if (i < 0 || i >= j->data_size) return -1;
	if (i == j->data_size - 1) {
		j->data_size--;
		return j->data[j->data_size];
	} else {
		j->log[j->log_size++] = i;
		return j->data[i];
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
