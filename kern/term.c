#include "ckern.h"
#include "scancodes.h"
#include "term.h"
#include "io.h"
#include "mem.h"
#include "string.h"
#include "command.h"
#define ROWS 25
#define COLS 80
#define VIDMEM (unsigned char*)0xB8000

/* this is horrific alcohol-induced crap in dire need of a redesign.
 * read at your own peril.
 * at least it works(tm)
 */

unsigned char ATTRIB = 0x1F; // white on blue, BSOD style.
char _curX = 0;
char _curY = 0;
extern void ClrScr32(void);
char cmdBuffer[1024];
unsigned short cmdIt = 0;
int tosAccepted = 0;
char toptwolowercase[]="##1234567890-=##qwertyuiop[]";
char toptwouppercase[]="##!@#$%^&*()_+##QWERTYUIOP{}";
char secondrowlowercase[]="asdfghjkl;'`";
char secondrowuppercase[]="ASDFGHJKL:\"¬";
char thirdrowlowercase[]="zxcvbnm,./";
char thirdrowuppercase[]="ZXCVBNM<>?";
int tempcaps = 0;
int capslock = 0;
char nums[10] = {'0','1','2','3','4','5','6','7','8','9'};
char numsx[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void MovCur(unsigned char x, unsigned char y) {
  unsigned short pos = y * COLS + x;
  outb(0x3D4, 0x0F);
  outb(0x3D5, (unsigned char)(pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}
inline void setCursorToCurrentPos() {
  MovCur(_curX, _curY);
}
void clearScreen() {
  ClrScr32();
  _curX = 0;
  _curY = 0;
  setCursorToCurrentPos();
}

// backspace, if we hit the start of the line we go to the last one, back to one space in front of the last character on that line
// spaces may wreak havoc on the command iterator, i can't remember if i accounted for that
void backspace() {
  if (cmdIt > 0) {
    _curX--;
    if (_curX < 0) {
      _curX = COLS;
      _curY--;
      if (_curY < 0) {
        _curY = 0;
      }
      unsigned int offset = (((_curY * COLS) * 2) + (_curX * 2));
      char c;
      while ((c = *(VIDMEM + offset)) == 0x00) {
        offset -= 2;
        _curX--;
        if (_curX == 0 || offset == 0) {
          break;
        }
      }
      if ((c = *(VIDMEM + offset)) != 0x00) {
        _curX++;
      }
    }
    unsigned int offset = (((_curY * COLS) * 2) + (_curX * 2));
    *(VIDMEM + offset) = 0x00;
    *(VIDMEM + offset + 1) = ATTRIB;
    setCursorToCurrentPos();
    cmdIt--;
  }
}
// Prints a single character and sets _curX and _curY respectively
void putch(const char c) {
  if (c == '\n') {
    _curY++;
    _curX = 0;
    if (_curY == ROWS) {
      scrollDown(1);
    }
    setCursorToCurrentPos();
  }
  else {
    if (_curX == COLS) {
      _curY++;
      _curX = 0;
    }
    if (_curY == ROWS) {
      scrollDown(1);
    }
    unsigned int offset = (((_curY * COLS) * 2) + (_curX * 2));
    *(VIDMEM + offset) = c;
    *(VIDMEM + offset + 1) = ATTRIB;
    _curX++;
  }
  return;
}

void cmdPrompt() {
  putch('\n');
  puts("kern@ligmos:/> ");
}
// Prints a null-terminated string
void puts(const char* s) {
  unsigned int i = 0;
  unsigned char c = *(s+i);
  while(c != 0) {
    putch(c);
    i++;
    c = *(s+i);
  }
  setCursorToCurrentPos();
  return;
}
// prints an integer
// because the digit characters are found in order of least to most significant digit
// printing them as-is will give us a backward number

void putint(int i) {
  int numchars = 0;
  char chars[128];
  // buffer the chars so we can print them in reverse
  while (i) {
    chars[numchars] = nums[i % 10];
    i /= 10;
    numchars++;
  }
  numchars--;
  while (numchars >= 0) {
    putch(chars[numchars]);
    numchars--;
  }
}
void putintx(unsigned int i) {
  int numchars = 0;
  char chars[128];
  puts("0x");
  // buffer the chars so we can print them in reverse
  if (i == 0) {
    chars[0] = '0';
	numchars = 1;
  }
  while (i) {
    chars[numchars] = numsx[i % 16];
    i /= 16;
    numchars++;
  }
  numchars--;
  while (numchars >= 0) {
    putch(chars[numchars]);
    numchars--;
  }
}
void writeLine(const char* s) {
  puts(s);
  putch('\n');
}
// we don't have an off-screen buffer yet, so this cannot happen
void scrollUp(const int lines) {
  writeLine("WARNING: scrollUp is not implemented.");
}
// shift all lines up by one row. discard bottom row.
// i'm not commenting this, you'll figure it out.
void scrollDown(const int lines) {
  for (int a = 0; a < lines; a++) {
    for (int i = 1; i < ROWS; i++) {
      for (int j = 0; j < COLS; j++) {
        unsigned int offset = (((i * COLS) * 2) + (j * 2));
        unsigned int offset2 = offset - (COLS * 2);
        *(VIDMEM + offset2) = *(VIDMEM + offset);
        *(VIDMEM + offset2 + 1) = *(VIDMEM + offset + 1);
      }
    }
    for (int col = 0; col < COLS; col++) {
      unsigned int offset = ((((ROWS - 1) * COLS) * 2) + (col * 2));
      *(VIDMEM + offset) = 0x00;
    }
  }
  _curY -= lines;
  if (_curY < 0) {
    _curY = 0;
    _curX = 0;
  }
  setCursorToCurrentPos();
}
void processCommand() {
  if (cmdIt > 0) {
	int cmdKnown = 0;
	cmdBuffer[cmdIt] = 0x00; // put a null terminator at the end
	for (int i = 0; i < CMDS_MAX; i++) {
		if (strcmp(cmdBuffer, commands[i].cmdText) == 0) {
			commands[i].execute("");
			cmdKnown = 1;
			break;
		}
	}
	if (cmdKnown == 0) {
		puts("Unknown command \"");
		puts(cmdBuffer);
		puts("\"\n");
	}
	cmdIt = 0;
  }
  cmdPrompt();
}
void addCharToCmdBufferAndPrint(char c) {
  if (cmdIt < 1024) {
    cmdBuffer[cmdIt] = c;
    putch(c);
    cmdIt++;
  }
}

void kbd_input(int scancode) {
  // first 2 rows (numbers then letters)
  if (scancode > 1 && scancode < 28) {
    // first don't do anything if this is a non-character key
    if (toptwolowercase[scancode] != '#') {
      if ((capslock ^ tempcaps) == 1) {
        addCharToCmdBufferAndPrint(toptwouppercase[scancode]);
      }
      else {
        addCharToCmdBufferAndPrint(toptwolowercase[scancode]);
      }
    }
  }
  // second row
  if (scancode > 29 && scancode < 42) {
    if ((capslock ^ tempcaps) == 1) {
      addCharToCmdBufferAndPrint(secondrowuppercase[scancode - 30]);
    }
    else {
      addCharToCmdBufferAndPrint(secondrowlowercase[scancode - 30]);
    }
  }
  // third row
  if (scancode > 43 && scancode < 54) {
    if ((capslock ^ tempcaps) == 1) {
      addCharToCmdBufferAndPrint(thirdrowuppercase[scancode - 44]);
    }
    else {
      addCharToCmdBufferAndPrint(thirdrowlowercase[scancode - 44]);
    }
  }
  // special keys
  if (scancode == SC_CAPSLOCK_DOWN) {
    if (capslock == 1) {
      capslock = 0;
      return;
    }
    capslock = 1;
    return;
  }
  if (scancode == SC_SHIFT_DOWN || scancode == SC_LSHIFT_DOWN) {
    tempcaps = 1;
    return;
  }
  if (scancode == SC_SHIFT_UP || scancode == SC_LSHIFT_UP) {
    tempcaps = 0;
    return;
  }
  if (scancode == SC_SPACE_DOWN) {
    addCharToCmdBufferAndPrint(' ');
  }
  if (scancode == SC_ENTER_DOWN) {
    putch('\n');
    processCommand();
  }
  if (scancode == SC_BACKSPACE_DOWN) {
    backspace();
  }
  setCursorToCurrentPos();
}

void initTerm() {
  puts("Welcome to ligmOS ");
  writeLine(VERSION);
  //writeLine("Copyright (C) 2018 Kole \"Spooky\"");
  //writeLine("ligmOS comes with ABSOLUTELY NO WARRANTY; type `warranty` for details");
  //writeLine("This is free software, and you are welcome to redistribute it");
  //writeLine("under certain conditions; type `license` for details");
  writeLine("32 bit paged protected mode\nKernel is mapped at 0xC0000000 (ELF header 0xC0000000-0xC0000FFF)");
  initCmds();
}
