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
  [_KEYS_1] = '1', [_KEYS_2] = '2', [_KEYS_3] = '3', [_KEYS_4] = '4', 
  [_KEYS_5] = '5', [_KEYS_6] = '6', [_KEYS_7] = '7', [_KEYS_8] = '8',
  [_KEYS_9] = '9', [_KEYS_0] = '0', [_KEYS_MINUS] = '-', [_KEYS_EQUALS] = '=',
  [_KEYS_TAB] = '\t', [_KEYS_Q] = 'q', [_KEYS_W] = 'w', [_KEYS_E] = 'e',
  [_KEYS_R] = 'r', [_KEYS_T] = 't', [_KEYS_Y] = 'y', [_KEYS_U] = 'u',
  [_KEYS_I] = 'i', [_KEYS_O] = 'o', [_KEYS_P] = 'p', [_KEYS_LEFTBRACKET] = '[',
  [_KEYS_RIGHTBRACKET] = ']', [_KEYS_BACKSLASH] = '\\', [_KEYS_A] = 'a',
  [_KEYS_S] = 's', [_KEYS_D] = 'd', [_KEYS_F] = 'f', [_KEYS_G] = 'g',
  [_KEYS_H] = 'h', [_KEYS_J] = 'j', [_KEYS_K] = 'k', [_KEYS_L] = 'l',
  [_KEYS_SEMICOLON] = ';', [_KEYS_APOSTROPHE] = '\'',
  [_KEYS_Z] = 'z', [_KEYS_X] = 'x', [_KEYS_C] = 'c', [_KEYS_V] = 'v',
  [_KEYS_B] = 'b', [_KEYS_N] = 'n', [_KEYS_M] = 'm', [_KEYS_COMMA] = ',',
  [_KEYS_PERIOD] = '.', [_KEYS_SLASH] = '/', [_KEYS_SPACE] = ' '
};

static const char keymap_upper[256] = {
  [_KEYS_1] = '!', [_KEYS_2] = '@', [_KEYS_3] = '#', [_KEYS_4] = '$', 
  [_KEYS_5] = '%', [_KEYS_6] = '^', [_KEYS_7] = '&', [_KEYS_8] = '*',
  [_KEYS_9] = '(', [_KEYS_0] = ')', [_KEYS_MINUS] = '_', [_KEYS_EQUALS] = '+',
  [_KEYS_TAB] = '\t', [_KEYS_Q] = 'Q', [_KEYS_W] = 'W', [_KEYS_E] = 'E',
  [_KEYS_R] = 'R', [_KEYS_T] = 'T', [_KEYS_Y] = 'Y', [_KEYS_U] = 'U',
  [_KEYS_I] = 'I', [_KEYS_O] = 'O', [_KEYS_P] = 'P', [_KEYS_LEFTBRACKET] = '{',
  [_KEYS_RIGHTBRACKET] = '}', [_KEYS_BACKSLASH] = '|', [_KEYS_A] = 'A',
  [_KEYS_S] = 'S', [_KEYS_D] = 'D', [_KEYS_F] = 'F', [_KEYS_G] = 'G',
  [_KEYS_H] = 'H', [_KEYS_J] = 'J', [_KEYS_K] = 'K', [_KEYS_L] = 'L',
  [_KEYS_SEMICOLON] = ':', [_KEYS_APOSTROPHE] = '\"',
  [_KEYS_Z] = 'Z', [_KEYS_X] = 'X', [_KEYS_C] = 'C', [_KEYS_V] = 'V',
  [_KEYS_B] = 'B', [_KEYS_N] = 'N', [_KEYS_M] = 'M', [_KEYS_COMMA] = '<',
  [_KEYS_PERIOD] = '>', [_KEYS_SLASH] = '?', [_KEYS_SPACE] = ' '
};

static const char (*keymap)[256] = {keymap_lower, keymap_upper};

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
    if (reg.keycode == _KEY_NONE) return 0;
    if (reg.keycode == _KEY_LSHIFT || reg.keycode == _KEY_RSHIFT) {
      if (reg.keydown) shiftcnt++;
      else shiftcnt--;
    } else if (reg.keydown) {
      switch (reg.keycode) {
        case _KEYS_CAPSLOCK: 
          capslock ^= 1;
          break;
        case _KEYS_RETURN:
          ret = 1;
          return '\n'
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

