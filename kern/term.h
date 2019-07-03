#ifndef _H_TERM_
#define _H_TERM_
void clearScreen();
void putch(const char);
void puts(const char*);
void putint(int i);
void backspace();
void writeLine(const char*);
void initTerm();
void scrollUp(const int);
void scrollDown(const int);
void kbd_input(int scancode);
void setCursorToCurrentPos();
void cmdPrompt();
unsigned char ATTRIB;
#endif