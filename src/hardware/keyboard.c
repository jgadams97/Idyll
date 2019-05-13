char KB_PIN_DATA;
char KB_PIN_CLK;
char KB_BUFFER[20];
unsigned short KB_RANDOM;

//Write a digital pin (open collector).
void kbDigitalWrite(char pin, char val) {
	if (val == LOW) {
		digitalWrite(pin, LOW);
		pinMode(pin, OUTPUT);
	} else {
		pinMode(pin, INPUT_PULLUP);
	}
}

//Setup keyboard.
void kbSetup(char pinDATA, char pinCLK) {
	KB_PIN_DATA = pinDATA;
	KB_PIN_CLK = pinCLK;
	kbDigitalWrite(KB_PIN_CLK, LOW);
	kbDigitalWrite(KB_PIN_DATA, LOW);
	delay(10);
	kbDigitalWrite(KB_PIN_CLK, LOW);
	kbDigitalWrite(KB_PIN_DATA, HIGH);
}

//Read a digital pin (open collector).
char kbDigitalRead(char pin) {
	pinMode(pin, INPUT_PULLUP);
	return digitalRead(pin);
}

char kbRead() {
	
	char pos = 0;
	
	for (char i = 0; i < 11; i++) {
		while (!kbDigitalRead(KB_PIN_CLK))
			KB_RANDOM++;
		while (kbDigitalRead(KB_PIN_CLK))
			KB_RANDOM++;
		KB_BUFFER[pos++] = kbDigitalRead(KB_PIN_DATA);
	}
	
	char data = 0;
	for (char i = 8; i >= 1; i--) {
		data <<= 1;
		data |= KB_BUFFER[i];
	}
	
	return data;
}

char KB_SHIFT = 0;
char kbGetChar() {
	kbDigitalWrite(KB_PIN_CLK, HIGH);
	char c;
	char extended = 0;
	while (1) {
		KB_RANDOM++;
		c = kbRead();
		if (EEPROM.read(c) == 14 || EEPROM.read(c) == 15) {
			KB_SHIFT = 1;
			continue;
		}
		if (c == (char)0xF0) {
			c = kbRead();
			if (EEPROM.read(c) == 14 || EEPROM.read(c) == 15) {
				KB_SHIFT = 0;
				continue;
			}
			break;
		}
		if (c == (char)0xE0) {
			c = kbRead();
			if (c == (char)0xF0) {
				c = kbRead();
				extended = 1;
				break;
			}
		}
	}
	
	kbDigitalWrite(KB_PIN_CLK, LOW);
	if (extended)
		return EEPROM.read(c + 256);
	else if (KB_SHIFT)
		return EEPROM.read(c + 128);
	else
		return EEPROM.read(c);
}