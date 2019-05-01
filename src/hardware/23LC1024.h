/*
Send Data
	1. Pull CS low.
	2. Clock in data on rising edge.
	3. Pull CS high.

Enable Write Byte Mode
	1. Send WRMR instruction (0x01).
	2. Send "byte operation" (0x00).
	
Read Sequence
	1. Send READ instruction (0x03).
	2. Send 24-bit address (top 7 being "don't care" bits).
	3. Data shifted out of SO pin.
	
Write Sequence
	1. Send WRITE instruction (0x02).
	2. Send 24-bit address (top 7 being "don't care" bits).
	3. Data shifted in on SI pin.
	
*/ 

//#include "digitalWriteFast.h"
#define PIN_SI 7
#define PIN_SO 5
#define PIN_CS 4
#define PIN_SCK 6
//#define digitalWriteFast(A, B) ((B == HIGH) ? (PORTD |= (1 << A)) : (PORTD &= ~(1 << A)))
//#define digitalReadFast(A) (PORTD >> A) & 1

//Shift out a byte.
void SRAM_CLOCK_BYTE_OUT(char b) {
	for (char i = 0; i < 8; i++) {
		if (b & 0b10000000) {
			digitalWrite(PIN_SI, HIGH);
		} else {
			digitalWrite(PIN_SI, LOW);
		}
		digitalWrite(PIN_SCK, HIGH);
		digitalWrite(PIN_SCK, LOW);
		b = b << 1;
	}
}

//Shift in a byte.
char SRAM_CLOCK_BYTE_IN() {
	char b = 0;
	for (char i = 0; i < 8; i++) {
		b = b << 1;
		if (i != 0) {
			digitalWrite(PIN_SCK, HIGH);
			digitalWrite(PIN_SCK, LOW);
		}
		if (digitalRead(PIN_SO))
			b += 1;
	}
	return b;
}

char SRAM_READ_MODE() {
	digitalWrite(PIN_SCK, LOW);
	digitalWrite(PIN_CS, LOW);
	SRAM_CLOCK_BYTE_OUT(0x05);
	char c = SRAM_CLOCK_BYTE_IN();
	digitalWrite(PIN_CS, HIGH);
	return c;
}

void SRAM_WRITE_MODE(char m) {
	digitalWrite(PIN_SCK, LOW);
	digitalWrite(PIN_CS, LOW);
	SRAM_CLOCK_BYTE_OUT(0x01);
	SRAM_CLOCK_BYTE_OUT(m);
	digitalWrite(PIN_CS, HIGH);
}


//Initialize SRAM chip in Byte Write mode.
void SRAM_INIT() {
	pinMode(PIN_SO, INPUT);
	pinMode(PIN_SI, OUTPUT);
	pinMode(PIN_SCK, OUTPUT);
	pinMode(PIN_CS, OUTPUT);
	SRAM_WRITE_MODE(0x00);
}

//Writes byte b to address a.
void SRAM_WRITE(ibword a,  char b) {
	digitalWrite(PIN_CS, LOW);
	SRAM_CLOCK_BYTE_OUT(0x02);
	SRAM_CLOCK_BYTE_OUT( (a & 0b10000000000000000) >> 16);
	SRAM_CLOCK_BYTE_OUT( (a & 0b01111111100000000) >>  8);
	SRAM_CLOCK_BYTE_OUT( (a & 0b00000000011111111) );
	SRAM_CLOCK_BYTE_OUT(b);
	digitalWrite(PIN_CS, HIGH);
}

//Reads byte from address a.
char SRAM_READ(ibword a) {
	digitalWrite(PIN_CS, LOW);
	SRAM_CLOCK_BYTE_OUT(0x03);
	SRAM_CLOCK_BYTE_OUT( (a & 0b10000000000000000) >> 16);
	SRAM_CLOCK_BYTE_OUT( (a & 0b01111111100000000) >>  8);
	SRAM_CLOCK_BYTE_OUT( (a & 0b00000000011111111) );
	char b = SRAM_CLOCK_BYTE_IN();
	digitalWrite(PIN_CS, HIGH);
	return b;
}

