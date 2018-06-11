#include <am.h>

// the signal handling mechanism has not been implemented.
void abort(void) {
  _halt(1);
}

