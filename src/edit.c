void linesEditorCalcSizes(unsigned short line, unsigned short *sizes, char rows) {
	for (char i = 0; i < rows; i++) {
		unsigned short j = 0;
		while (readRAM(j + (line + i) * LINE_BUFF_MAX) != 0) {
			j++;
		}
		sizes[i] = j;
	}
}

unsigned short linesEditorCalcSize(unsigned short lines) {
	unsigned short size = 0;
	for (unsigned short i = 0; i < lines; i++) {
		char c = readRAM(i * LINE_BUFF_MAX);
		unsigned short j = 0;
		while (c != 0) {
			size++;
			j++;
			c = readRAM(i * LINE_BUFF_MAX + j);
		}
		size++;
	}
	return size;
}

short linesEditor(short lines, char rows, char cols) {
	unsigned short line = 0;
	char x = 0, y = 0, i, j;
	unsigned short sizes[rows];
	unsigned short shifts[rows];
	for (char i = 0; i < rows; i++) {
		shifts[i] = 0;
	}
	while (lines < 4) {
		writeRAM(LINE_BUFF_MAX * lines, 0);
		lines++;
	}
	linesEditorCalcSizes(0, sizes, rows);
	while (1) {	
		for (i = 0; i < rows; i++) {
			for (j = 0; j < cols; j++) {
				unsigned short pos = (j + shifts[i]) + (i + line) * LINE_BUFF_MAX;
				char c = readRAM(pos);
				
				lcdSetCursor(i, j);
				if (j >= sizes[i])
					lcdWriteData(' ');
				else if (sizes[i] - cols - shifts[i] + 1 > 0 && j == cols - 1)
					lcdWriteData(0x7E);
				else if (shifts[i] > 0 && j == 0)
					lcdWriteData(0x7F);
				else
					lcdWriteData(c == 0 ? ' ' : c);
			}
		}
		
		lcdSetCursor(y, x);
		char userInput = readChar();
		switch (userInput) {
			//Up.
			case 17:
				y--;
				if (y == (char)-1) {
					if (line > 0)
						line--;
					y++;
				}
				for (i = 0; i < rows; i++) {
					if (i != y)
						shifts[i] = 0;
				}
			break;
			//Left.
			case 18:
				x--;
				if (x == (char)-1) {
					if (shifts[y] > 0)
						shifts[y]--;
					x++;
				}
			break;
			//Down.
			case 19:
				y++;
				if (y == rows) {
					line++;
					if (line > lines - rows)
						line--;
					y--;
				}
				for (i = 0; i < rows; i++) {
					if (i != y)
						shifts[i] = 0;
				}
			break;
			//Right.
			case 20:
				x++;
				if (x == cols) {
					shifts[y]++;
					x--;
					if (shifts[y] + x >= sizes[y] + 1)
						shifts[y]--;
				}
			break;
			//Escape.
			case 27:
				lcdClear();
				return linesEditorCalcSize(lines);
			break;
			//No save.
			case '`':
				lcdClear();
				return -1;
			break;
			//Backspace.
			case 8:
				if (sizes[y] == 0 || (x == 0 && shifts[y] == 0)) {				
					line += y;
					for (unsigned short ro = line; ro <= lines; ro++) {
						unsigned short co = 0;
						char c;
						do {
							c = readRAM((1 + ro) * LINE_BUFF_MAX + co);
							writeRAM(ro * LINE_BUFF_MAX + co, c);
							co++;
						} while (c != 0);
					}
					lines--;
					line -= y;
				} else {
					for (i = x + shifts[y]; i <= x + shifts[y] + sizes[y]; i++) {
						writeRAM((line + y) * LINE_BUFF_MAX + i - 1,
							readRAM((line + y) * LINE_BUFF_MAX + i));
					}
					x--;
					if (x == (char)-1) {
						if (shifts[y] > 0)
							shifts[y]--;
						x++;
					}
				}
			break;
			//Delete.
			case 127:
				writeRAM( (line + y) * LINE_BUFF_MAX , 0);
			break;
			//Enter.
			case 10:
				line += y;
				for (unsigned short ro = lines - 1; ro > line && ro != (unsigned short)-1; ro--) {
					unsigned short co = 0;
					char c;
					do {
						c = readRAM(ro * LINE_BUFF_MAX + co);
						writeRAM((1 + ro) * LINE_BUFF_MAX + co, c);
						co++;
					} while (c != 0);
				}
				writeRAM( (line + 1) * LINE_BUFF_MAX, 0);
				lines++;
				line -= y;
				y++;
				if (y == rows) {
					line++;
					if (line > lines - rows)
						line--;
					y--;
				}
				for (i = 0; i < rows; i++) {
					if (i != y)
						shifts[i] = 0;
				}
			break;
			default:
				if (userInput == 0) break;
				if (sizes[y] >= LINE_BUFF_MAX - 1)
					break;
				for (i = sizes[y] + 1; i > x + shifts[y]; i--) {
					writeRAM((line + y) * LINE_BUFF_MAX + i,
						readRAM((line + y) * LINE_BUFF_MAX + i - 1));
				}
				writeRAM((line + y) * LINE_BUFF_MAX + x + shifts[y], userInput);
				x++;
				if (x == cols) {
					shifts[y]++;
					x--;
				}
			break;
		}
		while (lines < 4) {
			writeRAM(LINE_BUFF_MAX * lines, 0);
			lines++;
		}
		while (line + rows > lines && line != 0) {
			line--;
		}
		linesEditorCalcSizes(line, sizes, rows);
		if (x + shifts[y] >= sizes[y] + 1) {
			x = sizes[y] - shifts[y];
		}
	}
	
		
}
