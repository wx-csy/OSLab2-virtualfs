#include <ctype.h>

int islower(int ch) {
  return ch >= 'a' && ch <= 'z';
}

int isupper(int ch) {
  return ch >= 'A' && ch <= 'Z';
}

int isalpha(int ch) {
  return isupper(ch) || islower(ch);
}

int tolower(int ch) {
  if (!isupper(ch)) return ch;
  else return ch - 'A' + 'a';
}

int toupper(int ch) {
  if (!islower(ch)) return ch;
  else return ch - 'a' + 'A';
}

int isdigit(int ch) {
  return ch >= '0' && ch <= '9';
}

int isalnum(int ch) {
  return isalpha(ch) || isdigit(ch);  
}

int isxdigit(int ch) {
  if (isdigit(ch)) return 1;
  ch = tolower(ch);
  return ch >= 'a' && ch <= 'f';
}

int iscntrl(int ch) {
  return ch < 32 || ch == 127;
}

int isgraph(int ch) {
  return ch >= 33 && ch < 127;
}

int isspace(int ch) {
  return ch == ' ' || (ch >= 9 && ch < 14);
}

int isblank(int ch) {
  return ch == ' ' || ch == '\t';
}

int isprint(int ch) {
  return ch >= 32 && ch < 127;
}

int ispunct(int ch) {
  return (ch >= 33 && ch < 48) || (ch >= 58 && ch < 65)
      || (ch >= 91 && ch < 97) || (ch >= 123 && ch < 127);
}

