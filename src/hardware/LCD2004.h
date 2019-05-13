#define LCD_ROWS 4
#define LCD_COLS 20
char LCD_ROW;
char LCD_COL;
char PIN_LCD_CLK;
char PIN_LCD_DATA;
char PIN_LCD_RS;
char PIN_LCD_EN;
char LCD_DATA[LCD_COLS * LCD_ROWS];

void lcdSetByte(char b) {
	for (unsigned char i = 0; i < 8; i++) {
		if (b & 0b10000000)
			digitalWrite(PIN_LCD_DATA, HIGH);
		else
			digitalWrite(PIN_LCD_DATA, LOW);
		digitalWrite(PIN_LCD_CLK, HIGH);
		digitalWrite(PIN_LCD_CLK, LOW);
		b <<= 1;
	}
	digitalWrite(PIN_LCD_EN, HIGH);
	digitalWrite(PIN_LCD_EN, LOW);
}

void lcdWriteRegister(char b) {
	digitalWrite(PIN_LCD_RS, LOW);
	lcdSetByte(b);
}

void lcdWriteData(char b) {
	digitalWrite(PIN_LCD_RS, HIGH);
	lcdSetByte(b);
}

void lcdInit(char pinEN, char pinRS, char pinData, char pinClock) {
	PIN_LCD_EN = pinEN;
	PIN_LCD_RS = pinRS;
	PIN_LCD_DATA = pinData;
	PIN_LCD_CLK = pinClock;
	digitalWrite(PIN_LCD_CLK, LOW);
	digitalWrite(PIN_LCD_DATA, LOW);
	digitalWrite(PIN_LCD_RS, LOW);
	digitalWrite(PIN_LCD_EN, LOW);
	pinMode(PIN_LCD_CLK, OUTPUT);
	pinMode(PIN_LCD_DATA, OUTPUT);
	pinMode(PIN_LCD_RS, OUTPUT);
	pinMode(PIN_LCD_EN, OUTPUT);
	delay(100);
	lcdWriteRegister(0x30);
	delay(5);
	lcdWriteRegister(0x30);
	delay(1);
	lcdWriteRegister(0x30);
	delay(1);
	lcdWriteRegister(0x38);
	delay(1);
	lcdWriteRegister(0x08);
	delay(1);
	lcdWriteRegister(0x01);
	delay(3);
	lcdWriteRegister(0x06);
	delay(1);
	lcdWriteRegister(0x0F);
	delay(1);
	LCD_ROW = 0;
	LCD_COL = 0;
	for (int i = 0; i < LCD_ROWS * LCD_COLS; i++) {
		LCD_DATA[i] = ' ';
	}
}

void lcdCursor(char toggle) {
	if (toggle) lcdWriteRegister(0x0F);
	else lcdWriteRegister(0x0C);
}

void lcdCursorBlink(char blink) {
	if (blink) lcdWriteRegister(0x0F);
	else lcdWriteRegister(0x0E);
}

void lcdClear() {
	lcdWriteRegister(0x01);
	delay(1);
	lcdWriteRegister(0x02);
	delay(1);
	LCD_ROW = 0;
	LCD_COL = 0;
}

void lcdSetCursor(char row, char col) {
	switch (row) {
		case 0:
			lcdWriteRegister(0x80 + col);
			break;
		case 1:
			lcdWriteRegister(0xC0 + col);
			break;
		case 2:
			lcdWriteRegister(0x94 + col);
			break;
		case 3:
			lcdWriteRegister(0xD4 + col);
			break;
	}
}

void lcdScroll() {
	char tmpRow = LCD_ROW - 1;
	lcdClear();
	for (unsigned char y = 0; y < LCD_ROWS - 1; y++) {
		lcdSetCursor(y, 0);
		for (unsigned char x = 0; x < LCD_COLS; x++) {
			LCD_DATA[y * LCD_COLS + x] = LCD_DATA[(y + 1) * LCD_COLS + x];
			LCD_DATA[(y + 1) * LCD_COLS + x] = ' ';
			lcdWriteData(LCD_DATA[y * LCD_COLS + x]);
		}
	}
	LCD_ROW = tmpRow;
	lcdSetCursor(LCD_ROW, 0);
}

void lcdNewline() {
	LCD_ROW++;
	LCD_COL = 0;
	if (LCD_ROW == 4) {
		lcdScroll();
	} else {
		lcdSetCursor(LCD_ROW, 0);
	}
	
}

void lcdPutChar(char c) {
	int i;
	if (c == '\n') {
		lcdNewline();
	} else {
		lcdWriteData(c);
		LCD_DATA[LCD_ROW * LCD_COLS + LCD_COL] = c;
		LCD_COL++;
		if (LCD_COL == LCD_COLS) {
			LCD_COL = 0;
			LCD_ROW++;
			if (LCD_ROW == 4) {
				lcdScroll();
			} else {
				lcdSetCursor(LCD_ROW, 0);
			}
		}
	}
}

void lcdBackspace() {
	if (LCD_COL == 0) {
		LCD_COL = LCD_COLS - 1;
		LCD_ROW--;
	} else {
		LCD_COL--;
	}
	lcdSetCursor(LCD_ROW, LCD_COL);
}