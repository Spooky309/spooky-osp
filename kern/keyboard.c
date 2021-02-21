#include "keyboard.h"

char popBuffer() {
  if (!kbdStruc.charInBuf) {
    return;
  }
  bufIt--;
  if (kbdStruc.bufIt < 0) {
    kbdStruc.bufIt = 0;
    kbdStruc.charInBuf = 0;
  }
  return kbdStruc.buffer[kbdStruc.bufIt];
}
