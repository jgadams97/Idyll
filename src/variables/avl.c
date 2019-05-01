int unistrcmp(char *str_a, char *str_b) {
	for (int i = 0;; i++) {
		if (str_a[i] == 0 && str_b[i] == 0) {
			return 0;
		}
		if (str_a[i] == 0) {
			for (int j = i;; j++) {
				if (str_b[j] == 0) {
					return i - j;
				}
			}
		}
		if (str_b[i] == 0) {
			for (int j = i;; j++) {
				if (str_a[j] == 0) {
					return j - i;
				}
			}
		}
		if (str_a[i] != str_b[i]) {
			return str_a[i] - str_b[i];
		}	
	}
	return 0;	
}

void unimemcpy(void *vptr_a, void *vptr_b, int size) {
	char *ptr_a = (char*)vptr_a;
	char *ptr_b = (char*)vptr_b;
	for (int i = 0; i < size; i++) {
		*(ptr_a + i) = *(ptr_b + i);;
	}
}

void intToString(char *str, int num) {
	char pos = 0;
	bool negative = false;
	if (num == 0) {
		str[0] = '0';
		str[1] = 0;
		return;
	}
	if (num < 0) {
		num *= -1;
		negative = true;
	}
	while (num > 0) {
		str[pos++] = (num % 10) + '0';
		num /= 10;
	}
	if (negative) 
		str[pos++] = '-';
	str[pos] = 0;
	for (char i = 0; i < pos / 2; i++) {
		char a = str[i];
		char b = str[pos - i - 1];
		str[i] = b;
		str[pos - i - 1] = a;
	}
}


void floatToString(char *str, float num) {
	intToString(str, (int)num);
	char pos = -1;
	while (str[++pos] != 0);
	num = num - (int)num;
	if (num < 0) num *= -1;
	int places = 0;
	if (num != 0) {
		str[pos++] = '.';
		while (num != 0) {
			num *= 10;
			str[pos++] = (int)num + '0';
			num = num - (int)num;
			places++;
			if (places == 7) break;
		}
		
		
		str[pos] = 0;
	}
}

void printString(const char *str) {
	int pos = 0;
	char c = str[pos++];
	while (c != 0) {
		writeChar(c);
		c = str[pos++];
	}
}

void printInt(int num) {
	int pos = 0;
	char numBuff[11];
	intToString(numBuff, num);
	printString(numBuff);
}

void printFloat(float num) {
	int pos = 0;
	char numBuff[20];
	floatToString(numBuff, num);
	printString(numBuff);
}

bool isWS(char c) {
	if (c == ' ' || c == '\t' || c == '\r')
		return true;
	return false;
}

bool isEOL(char c) {
	if (c == '\n' || c == 0 || c == 255) return true;
	return false;
}

//Checks if a character is a letter.
bool isAlpha(char c) {
	if (c >= 'A' && c <= 'Z') return true;
	if (c >= 'a' && c <= 'z') return true;
	if (c == '_') return true;
	return false;
}

//Checks if a character is a number or a letter.
bool isAlphaNum(char c) {
	if (c >= 'A' && c <= 'Z') return true;
	if (c >= 'a' && c <= 'z') return true;
	if (c >= '0' && c <= '9') return true;
	if (c == '_') return true;
	if (c == '.') return true;
	return false;
}

//Checks if a character is a number.
bool isNum(char c) {
	if (c >= '0' && c <= '9') return true;
	if (c == '.') return true;
	return false;
}

//Checks if a character is an operator.
bool isOperator(char c) {
	if (c == '+') return true;
	if (c == '-') return true;
	if (c == '*') return true;
	if (c == '/') return true;
	if (c == '&') return true;
	if (c == '|') return true;
	if (c == '^') return true;
	if (c == '%') return true;
	if (c == '=') return true;
	if (c == '~') return true;
	if (c == '>') return true;
	if (c == '<') return true;
	return false;
}

//Create a new node.
AVL_Node newNode(word address, word size, char *key, word left, word right) {
	AVL_Node n;
	n.address = address;
	n.size = size;
	n.left = left;
	n.right = right;
	for (char i = 0; i < KEY_SIZE; i++) {
		n.key[i] = ' ';
	}
	for (char i = 0; i < KEY_SIZE; i++) {
		if (key[i] == 0) break;
		n.key[i] = key[i];
	}
	n.key[KEY_SIZE] = 0;
	n.height = 1;
	n.parent = undefined;
	return n;
}

//Fetch a node from RAM.
AVL_Node fetchNode(word pos) {

	char buff[sizeof(AVL_Node)];
	for (char i = 0; i < sizeof(AVL_Node); i++) {
		buff[i] = readRAM(i + pos);
	}
	
	/*writeRAM(64, (int)-128);
	Serial.print("]");
	Serial.print((int)readRAM(64));
	Serial.println("[");*/
	
	
	
	AVL_Node ret;
	unimemcpy(&ret, buff, sizeof(AVL_Node));
	return ret;
}

//Store a node into RAM.
void storeNode(AVL_Node *n) {
	char buff[sizeof(AVL_Node)];
	unimemcpy(buff, n, sizeof(AVL_Node));
	for (char i = 0; i < sizeof(AVL_Node); i++) {
		writeRAM(i + n->address, buff[i]);
	}
}

//Pushes a node into RAM.
//	Returns the address it was stored at.
word pushNode(AVL_Node *n) {
	n->address = AVL_END;
	AVL_END += sizeof(AVL_Node) + n->size;
	char buff[sizeof(AVL_Node)];
	unimemcpy(buff, n, sizeof(AVL_Node));
	for (char i = 0; i < sizeof(AVL_Node); i++) {
		writeRAM(i + n->address, buff[i]);
	}
	return n->address;
}


void updateHeight(AVL_Node *node) {
	int leftHeight, rightHeight, height;
	if (node->left == undefined) leftHeight = 0;
	else leftHeight = fetchNode(node->left).height;
	if (node->right == undefined) rightHeight = 0;
	else rightHeight = fetchNode(node->right).height;
	if (leftHeight > rightHeight) {
		node->height = leftHeight + 1;
	} else {
		node->height = rightHeight + 1;
	}
	storeNode(node);
}

void updateHeights(AVL_Node node) {
	while (1) {
		updateHeight(&node);
		if (node.parent != undefined) {
			node = fetchNode(node.parent);
			continue;
		}
		break;
	}
}

void caseLeftLeft(AVL_Node z) {
	//Fetch nodes.
	AVL_Node parent;
	parent.address = undefined;
	if (z.parent != undefined) parent = fetchNode(z.parent);
	
	AVL_Node y = fetchNode(z.left);
	AVL_Node T3;
	T3.address = undefined;
	if (y.right != undefined) {
		T3 = fetchNode(y.right);
	}
	
	//Update children.
	if (parent.address != undefined) {
		if (parent.left == z.address) {
			parent.left = y.address;
		} else {
			parent.right = y.address;
		}
	}
	z.left = T3.address;
	y.right = z.address;
	
	//Update parents.
	y.parent = parent.address;
	z.parent = y.address;
	T3.parent = z.address;
	
	//Store nodes.
	if (parent.address != undefined) {
		storeNode(&parent);
	}
	storeNode(&y);
	storeNode(&z);
	if (T3.address != undefined) {
		storeNode(&T3);
	}
	
	//Update global root.
	if (y.parent == undefined) {
		AVL_ROOT = y.address;
	}
	
	//Update heights.
	updateHeight(&z);
	updateHeights(y);
}

void caseRightRight(AVL_Node z) {
	//Fetch nodes.
	AVL_Node parent;
	parent.address = undefined;
	if (z.parent != undefined) parent = fetchNode(z.parent);
	
	AVL_Node y = fetchNode(z.right);
	AVL_Node T2;
	T2.address = undefined;
	if (y.left != undefined) {
		T2 = fetchNode(y.left);
	}
	
	//Update children.
	if (parent.address != undefined) {
		if (parent.left == z.address) {
			parent.left = y.address;
		} else {
			parent.right = y.address;
		}
	}
	z.right = T2.address;
	y.left = z.address;
	
	//Update parents.
	y.parent = parent.address;
	z.parent = y.address;
	T2.parent = z.address;
	
	//Store nodes.
	if (parent.address != undefined) {
		storeNode(&parent);
	}
	storeNode(&y);
	storeNode(&z);
	if (T2.address != undefined) {
		storeNode(&T2);
	}
	
	//Update global root.
	if (y.parent == undefined) {
		AVL_ROOT = y.address;
	}
	
	//Update heights.
	updateHeight(&z);
	updateHeights(y);
}

//Rotate according to LeftRight case.
void caseLeftRight(AVL_Node z) {
	//Fetch nodes.
	AVL_Node parent;
	parent.address = undefined;
	if (z.parent != undefined) parent = fetchNode(z.parent);
	
	AVL_Node y = fetchNode(z.left);
	AVL_Node x = fetchNode(y.right);
	AVL_Node T2, T3;
	T2.address = undefined;
	T3.address = undefined;
	if (x.left != undefined) {
		T2 = fetchNode(x.left);
	}
	if (x.right != undefined) {
		T3 = fetchNode(x.right);
	}

	//Update children.
	if (parent.address != undefined) {
		if (parent.left == z.address) {
			parent.left = x.address;
		} else {
			parent.right = x.address;
		}
	}
	x.left = y.address;
	x.right = z.address;
	y.right = T2.address;
	z.left = T3.address;
	
	//Update parents.
	x.parent = parent.address;
	y.parent = x.address;
	z.parent = x.address;
	T2.parent = y.address;
	T3.parent = z.address;
	
	
	//Store nodes.
	if (parent.address != undefined) {
		storeNode(&parent);
	}
	storeNode(&x);
	storeNode(&y);
	storeNode(&z);
	if (T2.address != undefined) {
		storeNode(&T2);
	}
	if (T3.address != undefined) {
		storeNode(&T3);
	}
	
	//Update global root.
	if (x.parent == undefined) {
		AVL_ROOT = x.address;
	}
	
	//Update heights.
	updateHeight(&y);
	updateHeight(&z);
	updateHeights(x);
}


void caseRightLeft(AVL_Node z) {
	//Fetch nodes.
	AVL_Node parent;
	parent.address = undefined;
	if (z.parent != undefined) parent = fetchNode(z.parent);
	
	AVL_Node y = fetchNode(z.right);
	AVL_Node x = fetchNode(y.left);
	AVL_Node T2, T3;
	T2.address = undefined;
	T3.address = undefined;
	if (x.left != undefined) {
		T2 = fetchNode(x.left);
	}
	if (x.right != undefined) {
		T3 = fetchNode(x.right);
	}
	
	//Update children.
	if (parent.address != undefined) {
		if (parent.left == z.address) {
			parent.left = x.address;
		} else {
			parent.right = x.address;
		}
	}
	x.left = z.address;
	x.right = y.address;
	z.right = T2.address;
	y.left = T3.address;
	
	//Update parents.
	x.parent = parent.address;
	y.parent = x.address;
	z.parent = x.address;
	T2.parent = z.address;
	T3.parent = y.address;
	
	//Store nodes.
	if (parent.address != undefined) {
		storeNode(&parent);
	}
	storeNode(&x);
	storeNode(&y);
	storeNode(&z);
	if (T2.address != undefined) {
		storeNode(&T2);
	}
	if (T3.address != undefined) {
		storeNode(&T3);
	}
	
	//Update global root.
	if (x.parent == undefined) {
		AVL_ROOT = x.address;
	}
	
	//Update heights.
	updateHeight(&y);
	updateHeight(&z);
	updateHeights(x);
	
}

//Calculate the balance factor of a node.
int getNodeBalance(AVL_Node *node) {
	int leftHeight, rightHeight;
	if (node->left == undefined) leftHeight = 0;
	else leftHeight = fetchNode(node->left).height;
	if (node->right == undefined) rightHeight = 0;
	else rightHeight = fetchNode(node->right).height;
	return leftHeight - rightHeight;
}

//Inserts a new node to a tree.
//	Returns the address of the node.
word insertNode(AVL_Node *node) {

	//Just insert the node if there are no nodes.
	if (AVL_END == 0) {
		node->address = AVL_ROOT;
		pushNode(node);
		return AVL_ROOT;
	}

	AVL_Node root = fetchNode(AVL_ROOT);
	AVL_Node unbalanced;
	unbalanced.address = undefined;
	word height = 0;
	word return_val = undefined;
	
	//Go to the bottom.
	while (1) {
		int comparison = unistrcmp(node->key, root.key);
		if (comparison == 0) {
			return undefined;
		}
		
		//Move down the tree.
		if (comparison > 0 && root.right != undefined) {
			root = fetchNode(root.right);
			height++;
			continue;
		} else if (comparison < 0 && root.left != undefined) {
			root = fetchNode(root.left);
			height++;
			continue;
		}
		//Insert node.
		node->parent = root.address;
		if (comparison > 0) root.right = pushNode(node);
		else if (comparison < 0) root.left = pushNode(node);
		if (comparison > 0) return_val = root.right;
		else if (comparison < 0) return_val = root.left;
		storeNode(&root);
		break;
	}
	
	//Go back up to the top.
	AVL_Node pnode;
	int balance, pbalance;
	for (word i = 1; i <= height + 1; i++) {
		if (i + 1 > root.height) {
			root.height = i + 1;
			storeNode(&root);
		}
		//Check if the balance is off.
		if (unbalanced.address == undefined) {
			pbalance = balance;
			balance = getNodeBalance(&root);
			if (balance >= 2 || balance <= -2) {
				unbalanced = fetchNode(root.address);
			}
		}
		
		if (root.parent == undefined) break; 
		root = fetchNode(root.parent);
	}
	
	//Fix balancing.
	if (unbalanced.address != undefined) {
		/*printf("UNBALANCED: %u\n", unbalanced.address);
		
		if (balance > 0 && pbalance > 0) printf("\tLL\n");
		if (balance > 0 && pbalance < 0) printf("\tLR\n");
		if (balance < 0 && pbalance < 0) printf("\tRR\n");
		if (balance < 0 && pbalance > 0) printf("\tRL\n");*/
		
		if (balance > 0 && pbalance > 0) caseLeftLeft(unbalanced);
		if (balance > 0 && pbalance < 0) caseLeftRight(unbalanced);
		if (balance < 0 && pbalance < 0) caseRightRight(unbalanced);
		if (balance < 0 && pbalance > 0) caseRightLeft(unbalanced);
		
	}

	
	return return_val;
}

//Finds a node's address from a given key.
word findNode(char *tkey) {
	//Fix spacing for search.
	char key[KEY_SIZE + 1];
	for (char i = 0; i < KEY_SIZE; i++) {
		key[i] = ' ';
	}
	for (char i = 0; i < KEY_SIZE; i++) {
		if (tkey[i] == 0) break;
		key[i] = tkey[i];
	}
	key[KEY_SIZE] = 0;
	
	//Don't bother searching if there are no nodes.
	if (AVL_END == 0) return undefined;
	
	//Fetch the root node.
	AVL_Node root = fetchNode(AVL_ROOT);
	AVL_Node unbalanced;
	unbalanced.address = undefined;
	word height = 0;
	
	//Go to the bottom.
	while (1) {
		int comparison = unistrcmp(key, root.key);
		if (comparison == 0) {
			return root.address;
		}
		
		//Move down the tree.
		if (comparison > 0 && root.right != undefined) {
			root = fetchNode(root.right);
			continue;
		} else if (comparison < 0 && root.left != undefined) {
			root = fetchNode(root.left);
			continue;
		}
		
		//Nothing found.
		return undefined;
	}
}

//Print a node.
void printNode(AVL_Node n) {
	/*if (n.address == -1) printString("Address: undefined\n");
	else printf("Address: %u\n", n.address);
	printf("\tKey: |%s|\n", n.key);
	if (n.left == -1) printString("\tLeft child: undefined\n");
	else printf("\tLeft child: %u\n", n.left);
	if (n.right == -1) printf("\tRight child: undefined\n");
	else printf("\tRight child: %u\n", n.right);
	if (n.height == -1) printf("\tHeight: undefined\n");
	else printf("\tHeight: %u\n", n.height);
	//printf("\tBalance: %i\n", n.balance);
	if (n.size == -1) printf("\tSize: undefined\n");
	else printf("\tSize: %u\n", n.size);*/
}

void printTree(word pos) {
	AVL_Node n = fetchNode(pos);
	printNode(n);
	if (n.left != undefined) printTree(n.left);
	if (n.right != undefined) printTree(n.right);
}

//Verifies that a key is valid.
bool verifyKey(char *key) {
	char p = 0;
	char c = key[p++];
	if (c == '@' || c == '$') {
		c = key[p++];
	}
	if (c == 0) return false;
	if (!isAlpha(c)) return false;
	c = key[p++];
	while (c != 0) {
		if (!isAlphaNum(c)) return false;
		c = key[p++];
	}
	if (p > KEY_SIZE) return false;
	return true;
}
