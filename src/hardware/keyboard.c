char KB_PIN_DATA;
char KB_PIN_CLK;
#define KB_TIMEOUT 1000


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
}

//Read a digital pin (open collector).
char kbDigitalRead(char pin) {
	pinMode(pin, INPUT_PULLUP);
	return digitalRead(pin);
}

void kbSendByte(char val) {
	//Generate buffer.
	char buf[9];
	
	//Parity bit.
	buf[8] = 1;
	
	for (char i = 7; i >= 0; i--) {
		buf[i] = val >> 7;
		if (val >> 7) buf[8] ^= 1;
		val <<= 1;
	}
	
	//Request-to-send
	kbDigitalWrite(KB_PIN_CLK, LOW);
	delay(1);
	kbDigitalWrite(KB_PIN_DATA, LOW);
	
	for (int i = 0; i < 9; i++) {
		while (kbDigitalRead(KB_PIN_CLK));
		while (!kbDigitalRead(KB_PIN_CLK));
		kbDigitalWrite(KB_PIN_DATA, buf[i]);
	}
	
	//Wait for data line and clock low.
	while (kbDigitalRead(KB_PIN_DATA));
	while (kbDigitalRead(KB_PIN_CLK));
	
		
}

char kbRead() {
	
	char buf[11];
	char pos = 0;
	
	for (char i = 0; i < 11; i++) {
		while (!kbDigitalRead(KB_PIN_CLK));
		while (kbDigitalRead(KB_PIN_CLK));
		buf[pos++] = kbDigitalRead(KB_PIN_DATA);
	}
	
	char data = 0;
	for (char i = 8; i >= 1; i--) {
		data <<= 1;
		data |= buf[i];
	}
	
	return data;
}

char KB_SHIFT = 0;
char KB_CAPS = 0;
char kbGetChar() {
	char c;
	char extended = 0;
	while (1) {
		c = kbRead();
		if (EEPROM.read(c) == 14 || EEPROM.read(c) == 15) {
			KB_SHIFT = 1;
			continue;
		}
		if (c == (char)0xF0) {
			c = kbRead();
			if (c == (char)0x58) {
				KB_CAPS = !KB_CAPS;
				char resend = 1;
				while (resend) {
					resend = 0;
					kbSendByte(0xED);
					if (kbRead() == 0xFE)
						resend = 1;
				}
				while (resend) {
					resend = 0;
					kbSendByte(KB_CAPS << 2);
					if (kbRead() == 0xFE)
						resend = 1;
				}
				continue;
			}
			if (c == (char)0xE0) {
				c = kbRead();
				extended = 1;
			}
			
			if (EEPROM.read(c) == 14 || EEPROM.read(c) == 15) {
				KB_SHIFT = 0;
				continue;
			}
			break;
		}
	}
	if (extended)
		return EEPROM.read(c + 256);
	else if (KB_SHIFT || KB_CAPS)
		return EEPROM.read(c + 128);
	else
		return EEPROM.read(c);
}