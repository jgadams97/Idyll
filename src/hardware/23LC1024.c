
//Shift out a byte.
void SRAM_CLOCK_BYTE_OUT(char b) {
	for (char i = 0; i < 8; i++) {
		if (b & 0b10000000) {
			digitalWrite(SRAM_PIN_SI, HIGH);
		} else {
			digitalWrite(SRAM_PIN_SI, LOW);
		}
		digitalWrite(SRAM_PIN_SCK, HIGH);
		digitalWrite(SRAM_PIN_SCK, LOW);
		b = b << 1;
	}
}

//Shift in a byte.
char SRAM_CLOCK_BYTE_IN() {
	char b = 0;
	for (char i = 0; i < 8; i++) {
		b = b << 1;
		if (i != 0) {
			digitalWrite(SRAM_PIN_SCK, HIGH);
			digitalWrite(SRAM_PIN_SCK, LOW);
		}
		if (digitalRead(SRAM_PIN_SO))
			b += 1;
	}
	return b;
}

//Set read mode.
char SRAM_READ_MODE() {
	digitalWrite(SRAM_PIN_SCK, LOW);
	digitalWrite(SRAM_PIN_CS, LOW);
	SRAM_CLOCK_BYTE_OUT(0x05);
	char c = SRAM_CLOCK_BYTE_IN();
	digitalWrite(SRAM_PIN_CS, HIGH);
	return c;
}

//Set write mode.
void SRAM_WRITE_MODE(char m) {
	digitalWrite(SRAM_PIN_SCK, LOW);
	digitalWrite(SRAM_PIN_CS, LOW);
	SRAM_CLOCK_BYTE_OUT(0x01);
	SRAM_CLOCK_BYTE_OUT(m);
	digitalWrite(SRAM_PIN_CS, HIGH);
}


//Initialize SRAM chip in Byte Write mode.
void sramSetup(char pinCS, char pinSCK, char pinSO, char pinSI) {
	SRAM_PIN_CS = pinCS;
	SRAM_PIN_SCK = pinSCK;
	SRAM_PIN_SO = pinSO;
	SRAM_PIN_SI = pinSI;
	pinMode(SRAM_PIN_SO, INPUT);
	pinMode(SRAM_PIN_SI, OUTPUT);
	pinMode(SRAM_PIN_SCK, OUTPUT);
	pinMode(SRAM_PIN_CS, OUTPUT);
	SRAM_WRITE_MODE(0x00);
}

//Writes byte b to address a.
void sramWrite(long a,  char b) {
	digitalWrite(SRAM_PIN_CS, LOW);
	SRAM_CLOCK_BYTE_OUT(0x02);
	SRAM_CLOCK_BYTE_OUT( (a & 0b10000000000000000) >> 16);
	SRAM_CLOCK_BYTE_OUT( (a & 0b01111111100000000) >>  8);
	SRAM_CLOCK_BYTE_OUT( (a & 0b00000000011111111) );
	SRAM_CLOCK_BYTE_OUT(b);
	digitalWrite(SRAM_PIN_CS, HIGH);
}

//Reads byte from address a.
char sramRead(long a) {
	digitalWrite(SRAM_PIN_CS, LOW);
	SRAM_CLOCK_BYTE_OUT(0x03);
	SRAM_CLOCK_BYTE_OUT( (a & 0b10000000000000000) >> 16);
	SRAM_CLOCK_BYTE_OUT( (a & 0b01111111100000000) >>  8);
	SRAM_CLOCK_BYTE_OUT( (a & 0b00000000011111111) );
	char b = SRAM_CLOCK_BYTE_IN();
	digitalWrite(SRAM_PIN_CS, HIGH);
	return b;
}

