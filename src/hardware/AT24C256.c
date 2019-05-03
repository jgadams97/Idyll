//Write to EEPROM.
void eepromWrite(long addr, char val) {
	EEPROM_STATUS = 1;
	i2cBeginTransmission();
	i2cSend(0b10100000 | EEPROM_ADDRESS);
	if (i2cAcknowledge() != 0) {
		EEPROM_STATUS = 0;
		return;
	}
	i2cSend(addr >> 8);
	if (i2cAcknowledge() != 0) {
		EEPROM_STATUS = 0;
		return;
	}
	i2cSend(addr & 0xFF);
	if (i2cAcknowledge() != 0) {
		EEPROM_STATUS = 0;
		return;
	}
	i2cSend(val);
	if (i2cAcknowledge() != 0) {
		EEPROM_STATUS = 0;
		return;
	}
	i2cEndTransmission();
	delay(5);
}

//Read from EEPROM.
char eepromRead(long addr) {
	EEPROM_STATUS = 1;
	char val = 0;
	i2cBeginTransmission();
	i2cSend(0b10100000 | EEPROM_ADDRESS);
	if (i2cAcknowledge() != 0) {
		EEPROM_STATUS = 0;
		return 0;
	}
	i2cSend(addr >> 8);
	if (i2cAcknowledge() != 0) {
		EEPROM_STATUS = 0;
		return 0;
	}
	i2cSend(addr & 0xFF);
	if (i2cAcknowledge() != 0) {
		EEPROM_STATUS = 0;
		return 0;
	}

	i2cBeginTransmission();
	i2cSend(0b10100001 | EEPROM_ADDRESS);
	if (i2cAcknowledge() != 0) {
		EEPROM_STATUS = 0;
		return 0;
	}
	val = i2cReceive();
	if (i2cAcknowledge() != 1) {
		EEPROM_STATUS = 0;
		return 0;
	}
	i2cEndTransmission();

	return val;	
}

//Get status of recent read/write.
char eepromSuccess() {
	return EEPROM_STATUS;
}

//Setup EEPROM.
char eepromSetup(char addr, int pinSDA, int pinSCL) {
	i2cSetup(pinSDA, pinSCL);
	EEPROM_ADDRESS = (addr << 1) & 0b110;
	eepromRead(0);
	return eepromSuccess();
}
