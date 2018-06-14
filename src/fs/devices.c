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

int dev_stdin_getch() {
  _KbdReg reg;
  while (1) {
    dev_input->read(_DEVREG_INPUT_KBD, &reg, sizeof reg);
    if (reg.keycode == _KEY_NONE) return 0;
    if (reg.keydown) {
      _putc(reg.keycode);
      return reg.keycode;
    }
  }
}

int dev_stdout_putch(int ch) {
  _putc(ch);
  return 0;
}

