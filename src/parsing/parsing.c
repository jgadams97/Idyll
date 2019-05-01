
#define TYPE_FLOAT 0
#define TYPE_INTEGER 1
#define TYPE_STRING 2
#define COMMAND_SIZE 8
byte COMMAND_BUFFER[COMMAND_SIZE];

//Compare to bytes, ignoring casing.
bool equalsIgnoreCasing(byte a, byte b) {
	if (a == b) return true;
	if (a - ( 'a' - 'A' ) == b) return true;
	if (a + ( 'a' - 'A' ) == b) return true;
	return false;
}

//Check if a command is located at a given index.
bool isCommand(byte command_index) {
	for (byte i = 0; i < COMMAND_SIZE; i++) {
		byte b = COMMAND_BUFFER[i];
		byte c = readROM(command_index * COMMAND_SIZE + i);
		if (c == 0) {
			if (b == ' ' || b == '\n' || b == 0) return true;
			else return false;
		} if (!equalsIgnoreCasing(b, c)) {
			return false;
		}
	}
	return false;
}

//Extracts a command from a position in RAM.
//  Returns the command's index.


//Gets argument count.
bool exec(word pos) {
	//Variables used...
	byte i = 0;
	byte valid = false;
	byte command = -1;
	byte c = 0;
	
	//Skip over white space.
	c = readPRG(pos);
	while ( (c == ' ' || c == '\t') && c != 0) {
		c = readPRG(++pos);
	}
	if (c == 0) return true;
	
	//Copy command into the command buffer.
	for (byte i = 0; i < 8; i++) {
		COMMAND_BUFFER[i] = 8;
	}
	
	//Look for command
	while (readROM(COMMAND_SIZE * i) != 255) {
		if (isCommand(i)) {
			command = i;
			break;
		}
		i++;
	}
	if (command == -1) return false;
	
	//Skip over command.
	for (i = 0; i < COMMAND_SIZE; i++) {
		if (readROM(COMMAND_SIZE * command) == 0) {
			break;
		}
		pos++;
	}
	
	//Skip over white space.
	c = readPRG(pos);
	while ( (c == ' ' || c == '\t') && c != 0 ) {
		c = readPRG(++pos);
	}
	
	
	//Parse arguments.
	
	
	return true;
	
}