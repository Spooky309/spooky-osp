#ifndef _H_KEYBOARD_
#define _H_KEYBOARD_
enum kbd_states {
  IDLE,
  WAIT_ACK,
  GET_SCANCODE
};
typedef struct kbd {
  int state = IDLE;
  unsigned char cmdbuffer[10];
  char buffer[255];
  unsigned char cmdInBuf;
  unsigned char charInBuf;
  short cmdIt = 0;
  short bufIt = 0;
} kbd_t;
kbd_t kbdStruc;
void initKeyboard();
void think();
char popBuffer();
#endif