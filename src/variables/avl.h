#include "../hardware/hardware.h"
#define KEY_SIZE 16
word AVL_ROOT = 0;
word AVL_END = 0;
typedef struct _avl_node {
	char key[KEY_SIZE + 1];
	word left;
	word right;
	word address;
	word size;
	word parent;
	int height;
} AVL_Node;

//Create a new node.
AVL_Node newNode(word address, word size, char *key, word left, word right);

//Fetch a node from RAM.
AVL_Node fetchNode(word pos);

//Store a node into RAM.
void storeNode(AVL_Node *n);

//Pushes a node into RAM.
//	Returns the address it was stored at.
word pushNode(AVL_Node *n);

//Update the height of a single node.
void updateHeight(AVL_Node *node);

//Update the height of a node and all its ancestors.
void updateHeights(AVL_Node node);

//AVL tree Left-Left case.
void caseLeftLeft(AVL_Node z);

//AVL tree Right-Right case.
void caseRightRight(AVL_Node z);

//AVL tree Left-Right case;
void caseLeftRight(AVL_Node z);

//AVL tree Right-Left case.
void caseRightLeft(AVL_Node z);

//Calculate the balance factor of a node.
int getNodeBalance(AVL_Node *node);

//Inserts a new node to a tree.
//	Returns the address of the node.
word insertNode(AVL_Node *node);

//Print a node.
//void printNode(AVL_Node n);

//Print an entire tree whose root is located as pos.
void printTree(word pos);

//Find a node from a given key.
word findNode(char *key);

//Verifies that a key for a node is valid.
bool verifyKey(char *key);

//Checks if a character is a letter.
bool isAlpha(char c);

//Checks if a character is a number or a letter.
bool isAlphaNum(char c);

//Checks if a character is a number.
bool isNum(char c);

//Checks if a character is an operator.
bool isOperator(char c);

//Checks if a character is a whitespace character.
bool isWS(char c);

//Checks if a character is an end of line character.
bool isEOL(char c);

#include "avl.c"