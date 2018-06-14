#include <kernel.h>
#include <fs/kvfs.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// #define DEBUG_ME
#include <debug.h>

// null device

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

