#include "../hardware/hardware_dynamic.h"
#define KEY_SIZE 16

typedef struct _avl_node {
	char key[KEY_SIZE + 1];
	ibword left;
	ibword right;
	ibword address;
	ibword size;
	ibword parent;
	ibword height;
	ibword next;
} AVL_Node;
typedef struct _dangling_node {
	ibword next;
	ibword size;
} AVL_Dangling;

//Create a new node.
AVL_Node newNode(ibword address, ibword size, char *key, ibword left, ibword right);

//Fetch a node from RAM.
AVL_Node fetchNode(Evaluator *e, ibword pos);

//Store a node ibwordo RAM.
void storeNode(Evaluator *e, AVL_Node *n);

//Pushes a node ibwordo RAM.
//	Returns the address it was stored at.
ibword pushNode(Evaluator *e, AVL_Node *n);

//Update the height of a single node.
void updateHeight(AVL_Node *node);

//Update the height of a node and all its ancestors.
void updateHeights(AVL_Node node);

//AVL tree Left-Left case.
void caseLeftLeft(Evaluator *e, AVL_Node z);

//AVL tree Right-Right case.
void caseRightRight(Evaluator *e, AVL_Node z);

//AVL tree Left-Right case;
void caseLeftRight(Evaluator *e, AVL_Node z);

//AVL tree Right-Left case.
void caseRightLeft(Evaluator *e, AVL_Node z);

//Calculate the balance factor of a node.
ibword getNodeBalance(Evaluator *e, AVL_Node *node);

//Inserts a new node to a tree.
//	Returns the address of the node.
ibword insertNode(AVL_Node *node);

//Pribword a node.
//void printNode(AVL_Node n);

//Pribword an entire tree whose root is located as pos.
void printTree(Evaluator *e, ibword pos);

//Find a node from a given key.
ibword findNode(Evaluator *e, char *key);

//Verifies that a key for a node is valid.
bool verifyKey(char *key);

//Checks if a character is a letter.
bool isAlpha(char c);

//Checks if a character is a number or a letter.
bool isAlphaNum(char c);

#include "avl_dynamic.c"
