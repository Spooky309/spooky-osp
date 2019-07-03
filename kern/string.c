#include "string.h"
// not to spec!
// strcmp should return int of how many characters in before a difference
// but we don't care right now, only if they are different
int strcmp(const char* s1, const char* s2) {
  char c1;
  char c2;
  unsigned int it = 0;
  int ident = 1;
  while ((c1 = s1[it]) != 0x00) {
    c2 = s2[it];
    if (c1 == c2) {
      ident = 0;
    }
    else {
      ident = 1;
      break;
    }
    it++;
  }
  if (s1[it] != s2[it]) {
    ident = 1;
  }
  return ident;
}
