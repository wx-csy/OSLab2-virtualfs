#include <stdlib.h>
#include <am.h>

void _Exit(int exit_code) {
  _halt(exit_code);
}

