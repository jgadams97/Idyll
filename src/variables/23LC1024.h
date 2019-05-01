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

#define PIN_SI 7
#define PIN_SO 5
#define PIN_CS 4
#define PIN_SCK 6

//Shift out a byte.
void SRAM_CLOCK_BYTE_OUT(unsigned char b) {
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
unsigned char SRAM_CLOCK_BYTE_IN() {
	unsigned char b = 0;
	for (char i = 0; i < 8; i++) {
		b = b << 1;
		if (digitalRead(PIN_SO) == HIGH) {
			b += 1;
		}
		digitalWrite(PIN_SCK, HIGH);
		digitalWrite(PIN_SCK, LOW);
	}
	return b;
}

//Initialize SRAM chip in Byte Write mode.
void SRAM_INIT() {
	pinMode(PIN_SO, INPUT);
	pinMode(PIN_SI, OUTPUT);
	pinMode(PIN_SCK, OUTPUT);
	pinMode(PIN_CS, OUTPUT);
	digitalWrite(PIN_SCK, LOW);
	digitalWrite(PIN_CS, HIGH);
	digitalWrite(PIN_CS, LOW);
	SRAM_CLOCK_BYTE_OUT(0x01);
	SRAM_CLOCK_BYTE_OUT(0x00);
	digitalWrite(PIN_CS, HIGH);
}

//Writes byte b to address a.
void SRAM_WRITE(int a, unsigned char b) {
	digitalWrite(PIN_SCK, LOW);
	digitalWrite(PIN_CS, LOW);
	SRAM_CLOCK_BYTE_OUT(0x02);
	SRAM_CLOCK_BYTE_OUT( (a & 0b10000000000000000) >> 16);
	SRAM_CLOCK_BYTE_OUT( (a & 0b01111111100000000) >>  8);
	SRAM_CLOCK_BYTE_OUT( (a & 0b00000000011111111) );
	SRAM_CLOCK_BYTE_OUT(b);
	digitalWrite(PIN_CS, HIGH);
}

//Reads byte from address a.
unsigned char SRAM_READ(int a) {
	digitalWrite(PIN_SCK, LOW);
	digitalWrite(PIN_CS, LOW);
	SRAM_CLOCK_BYTE_OUT(0x03);
	SRAM_CLOCK_BYTE_OUT( (a & 0b10000000000000000) >> 16);
	SRAM_CLOCK_BYTE_OUT( (a & 0b01111111100000000) >>  8);
	SRAM_CLOCK_BYTE_OUT( (a & 0b00000000011111111) );
	unsigned char b = SRAM_CLOCK_BYTE_IN();
	digitalWrite(PIN_CS, HIGH);
	return b;
}
