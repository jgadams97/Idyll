#include "avl.h"

//Creates a number variable.
void dimNum(char *name, float value) {
	
	//Create the node.
	AVL_Node n = newNode(undefined, sizeof(float), name, undefined, undefined);
	word address = insertNode(&n);
	
	//Store value into node.
	char buffer[sizeof(float)];
	unimemcpy(buffer, &value, sizeof(float));
	for (char i = 0; i < sizeof(float); i++) {
		writeRAM(address + sizeof(AVL_Node) + i, buffer[i]);
	}
	
}

//Creates an address variable.
void dimAdr(char *name, word value) {
	
	//Create the node.
	AVL_Node n = newNode(undefined, sizeof(word), name, undefined, undefined);
	word address = insertNode(&n);
	
	//Store value into node.
	char buffer[sizeof(word)];
	unimemcpy(buffer, &value, sizeof(word));
	for (char i = 0; i < sizeof(word); i++) {
		writeRAM(address + sizeof(AVL_Node) + i, buffer[i]);
	}
	
}

//Creates a string variable.
void dimStr(char *name, word size) {
	//Create the node.
	AVL_Node n = newNode(undefined, size, name, undefined, undefined);
	insertNode(&n);
}

//Reads a number variable at an address.
float readNum(word address) {;
	float value;	
	//Store value into node.
	char buffer[sizeof(float)];
	for (char i = 0; i < sizeof(float); i++) {
		buffer[i] = readRAM(address + sizeof(AVL_Node) + i);
	}
	unimemcpy(&value, buffer, sizeof(float));
	return value;
}

//Reads an address variable at an address.
word readAdr(word address) {;
	word value;	
	//Store value into node.
	char buffer[sizeof(word)];
	for (char i = 0; i < sizeof(word); i++) {
		buffer[i] = readRAM(address + sizeof(AVL_Node) + i);
	}
	unimemcpy(&value, buffer, sizeof(word));
	return value;
}

//Reads a character from a string variable at an address at a given index.
char readStr(word address, word index) {
	//Return value;
	return readRAM(address + sizeof(AVL_Node) + index);
}


//Reads a number variable at an address.
void writeNum(word address, float value) {
	//Store value into node.
	char buffer[sizeof(float)];
	unimemcpy(buffer, &value, sizeof(float));
	for (char i = 0; i < sizeof(float); i++) {
		writeRAM(address + sizeof(AVL_Node) + i, buffer[i]);
	}
}

//Reads a number variable at an address.
bool writeStr(word address, word index, char c) {
	//Writes character to string.
	writeRAM(address + sizeof(AVL_Node) + index, c);
}

//Reads size of a string.
word readStrSize(word address) {
	AVL_Node n = fetchNode(address);
	return n.size;
}
