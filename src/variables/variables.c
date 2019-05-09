#include "avl.h"

//Creates a number variable.
void dimNum(char *name, float value) {
	
	//Create the node.
	AVL_Node n = newNode(undefined, sizeof(float), name, undefined, undefined);
	ibword address = insertNode(&n);
	
	//Store value ibwordo node.
	char buffer[sizeof(float)];
	unimemcpy(buffer, &value, sizeof(float));
	for (char i = 0; i < sizeof(float); i++) {
		writeRAM(address + sizeof(AVL_Node) + i, buffer[i]);
	}
	
}

//Creates a number array.
void dimArr(char *name, ibword size) {
	
	//Create the node.
	AVL_Node n = newNode(undefined, size * sizeof(float), name, undefined, undefined);
	insertNode(&n);
	
}

//Creates an address variable.
void dimAdr(char *name, ibword value) {
	
	//Create the node.
	AVL_Node n = newNode(undefined, sizeof(ibword), name, undefined, undefined);
	ibword address = insertNode(&n);
	
	//Store value ibwordo node.
	char buffer[sizeof(ibword)];
	unimemcpy(buffer, &value, sizeof(ibword));
	for (char i = 0; i < sizeof(ibword); i++) {
		writeRAM(address + sizeof(AVL_Node) + i, buffer[i]);
	}
	
}

//Creates a string variable.
ibword dimStr(char *name, ibword size) {
	//Create the node.
	AVL_Node n = newNode(undefined, size, name, undefined, undefined);
	n.next = undefined;
	ibword addr = insertNode(&n);
	//If the size > 0, set the first value to 0.
	if (size > 0)
		writeRAM(addr + sizeof(AVL_Node), 0);
	return addr;
}

//Reads a number variable at an address.
float readNum(ibword address) {;
	float value;	
	//Store value ibwordo node.
	char buffer[sizeof(float)];
	for (char i = 0; i < sizeof(float); i++) {
		buffer[i] = readRAM(address + sizeof(AVL_Node) + i);
	}
	unimemcpy(&value, buffer, sizeof(float));
	return value;
}

//Creates a dangling node.
//These nodes are inserted into the list of nodes in the AVL
//	tree but no other nodes point to them.
//They can thus be used to store information between nodes
//	inside the AVL tree without interfering with the tree's
//	structure itself.
ibword insertDanglingNode(ibword size) {
	//Create the node.
	AVL_Node n = newNode(AVL_END, size, NULL, undefined, undefined);
	n.next = undefined;
	AVL_END += sizeof(AVL_Node) + size;
	storeNode(&n);
	return n.address;
}

//Reads an address variable at an address.
ibword readAdr(ibword address) {;
	ibword value;	
	//Store value ibwordo node.
	char buffer[sizeof(ibword)];
	for (char i = 0; i < sizeof(ibword); i++) {
		buffer[i] = readRAM(address + sizeof(AVL_Node) + i);
	}
	unimemcpy(&value, buffer, sizeof(ibword));
	return value;
}

//Reads a character from a string variable at an address at a given index.
char readStr(ibword address, ibword index) {
	AVL_Node n = fetchNode(address);
	while (index >= n.size) {
		index -= n.size;
		n = fetchNode(n.next);
	}
	//Return value.
	return readRAM(n.address + sizeof(AVL_Node) + index);
}

//Reads an array at an index.
float readArr(ibword address, ibword index) {
	//Return value;
	float ret;
	char *ptr = (char*)(&ret);
	for (char i = 0; i < sizeof(float); i++)
		ptr[i] = readRAM(address + sizeof(AVL_Node) + index * sizeof(float) + i);
	return ret;
}

//Writes an array at an index.
void writeArr(ibword address, ibword index, float val) {
	//Return value;
	char *ptr = (char*)(&val);
	for (char i = 0; i < sizeof(float); i++)
		writeRAM(address + sizeof(AVL_Node) + index * sizeof(float) + i, ptr[i]);
}


//Reads a number variable at an address.
void writeNum(ibword address, float value) {
	//Store value ibwordo node.
	char buffer[sizeof(float)];
	unimemcpy(buffer, &value, sizeof(float));
	for (char i = 0; i < sizeof(float); i++) {
		writeRAM(address + sizeof(AVL_Node) + i, buffer[i]);
	}
}

//Reads a number variable at an address.
bool writeStr(ibword address, ibword index, char c) {
	AVL_Node n = fetchNode(address);
	while (index >= n.size) {
		index -= n.size;
		n = fetchNode(n.next);
	}
	//Return value.
	writeRAM(n.address + sizeof(AVL_Node) + index, c);
}

//Reads size of a string.
ibword readStrSize(ibword address) {
	ibword size;
	AVL_Node n = fetchNode(address);
	size = n.size;
	while (n.next != (ibword)undefined) {
		n = fetchNode(n.next);
		size += n.size;
	}
	return size;
}

//Reads size of a string.
ibword readArrSize(ibword address) {
	AVL_Node n = fetchNode(address);
	return n.size / sizeof(float);
}
