#include <kernel.h>
#include <fs/kvfs.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <am.h>
#include <amdev.h>

// #define DEBUG_ME
#include <debug.h>

static _Device *dev_input = NULL;

void dev_init() {
  for (int n = 1; ; n++) {
    _Device *dev = _device(n);
    if (!dev) break;
    if (dev->id == _DEV_INPUT) {
      dev_input = dev;
    }
  }
  assert(dev_input != NULL);
}

int dev_null_getch() {
  return -1; 
}

int dev_null_putch(int ch) {
  return 0;
}

int dev_zero_getch() {
  return 0;
}

int dev_random_getch() {
  return rand() & 0xff;
}

static _Device *dev_input;

static const char keymap_lower[256] = {
  [_KEY_1] = '1', [_KEY_2] = '2', [_KEY_3] = '3', [_KEY_4] = '4', 
  [_KEY_5] = '5', [_KEY_6] = '6', [_KEY_7] = '7', [_KEY_8] = '8',
  [_KEY_9] = '9', [_KEY_0] = '0', [_KEY_MINUS] = '-', [_KEY_EQUALS] = '=',
  [_KEY_TAB] = '\t', [_KEY_Q] = 'q', [_KEY_W] = 'w', [_KEY_E] = 'e',
  [_KEY_R] = 'r', [_KEY_T] = 't', [_KEY_Y] = 'y', [_KEY_U] = 'u',
  [_KEY_I] = 'i', [_KEY_O] = 'o', [_KEY_P] = 'p', [_KEY_LEFTBRACKET] = '[',
  [_KEY_RIGHTBRACKET] = ']', [_KEY_BACKSLASH] = '\\', [_KEY_A] = 'a',
  [_KEY_S] = 's', [_KEY_D] = 'd', [_KEY_F] = 'f', [_KEY_G] = 'g',
  [_KEY_H] = 'h', [_KEY_J] = 'j', [_KEY_K] = 'k', [_KEY_L] = 'l',
  [_KEY_SEMICOLON] = ';', [_KEY_APOSTROPHE] = '\'',
  [_KEY_Z] = 'z', [_KEY_X] = 'x', [_KEY_C] = 'c', [_KEY_V] = 'v',
  [_KEY_B] = 'b', [_KEY_N] = 'n', [_KEY_M] = 'm', [_KEY_COMMA] = ',',
  [_KEY_PERIOD] = '.', [_KEY_SLASH] = '/', [_KEY_SPACE] = ' '
};

static const char keymap_upper[256] = {
  [_KEY_1] = '!', [_KEY_2] = '@', [_KEY_3] = '#', [_KEY_4] = '$', 
  [_KEY_5] = '%', [_KEY_6] = '^', [_KEY_7] = '&', [_KEY_8] = '*',
  [_KEY_9] = '(', [_KEY_0] = ')', [_KEY_MINUS] = '_', [_KEY_EQUALS] = '+',
  [_KEY_TAB] = '\t', [_KEY_Q] = 'Q', [_KEY_W] = 'W', [_KEY_E] = 'E',
  [_KEY_R] = 'R', [_KEY_T] = 'T', [_KEY_Y] = 'Y', [_KEY_U] = 'U',
  [_KEY_I] = 'I', [_KEY_O] = 'O', [_KEY_P] = 'P', [_KEY_LEFTBRACKET] = '{',
  [_KEY_RIGHTBRACKET] = '}', [_KEY_BACKSLASH] = '|', [_KEY_A] = 'A',
  [_KEY_S] = 'S', [_KEY_D] = 'D', [_KEY_F] = 'F', [_KEY_G] = 'G',
  [_KEY_H] = 'H', [_KEY_J] = 'J', [_KEY_K] = 'K', [_KEY_L] = 'L',
  [_KEY_SEMICOLON] = ':', [_KEY_APOSTROPHE] = '\"',
  [_KEY_Z] = 'Z', [_KEY_X] = 'X', [_KEY_C] = 'C', [_KEY_V] = 'V',
  [_KEY_B] = 'B', [_KEY_N] = 'N', [_KEY_M] = 'M', [_KEY_COMMA] = '<',
  [_KEY_PERIOD] = '>', [_KEY_SLASH] = '?', [_KEY_SPACE] = ' '
};

static const char *(keymap[2]) = {keymap_lower, keymap_upper};

int dev_stdin_getch() {
  _KbdReg reg;
  static int shiftcnt = 0, capslock = 0;
  static int ret = 0;
  char ch;
  if (ret) {
    ret = 0;
    return -2;
  }
  while (1) {
    dev_input->read(_DEVREG_INPUT_KBD, &reg, sizeof reg);
    if (reg.keycode) {
      _debug("keycode=%d, keypress=%d", reg.keycode, reg.keydown);
    }
    if (reg.keycode == _KEY_NONE) return 0;
    if (reg.keycode == _KEY_LSHIFT || reg.keycode == _KEY_RSHIFT) {
      if (reg.keydown) shiftcnt++;
      else shiftcnt--;
    } else if (reg.keydown) {
      switch (reg.keycode) {
        case _KEY_CAPSLOCK: 
          capslock ^= 1;
          break;
        case _KEY_RETURN:
          ret = 1;
          return '\n';
        default:
          ch = keymap[!shiftcnt ^ !capslock][reg.keycode];
          if (ch) {
            _putc(ch);
            return ch;
          }
      }
    }
  }
}

int dev_stdout_putch(int ch) {
  _putc(ch);
  return 0;
}

