#include <stdlib.h>
#include <stdint.h>

// an implementation of xorshift+ PRNG.

#define MAGIC1    0x3141592653589793LL
#define MAGIC2    0xFEDCBA9876543210LL
#define MAGIC3    0x0123456789ABCDEFLL
#define MAGIC4    0x1A4F2B8E5C7D2718LL

static uint64_t state[2] = {MAGIC1 + 1, MAGIC2};

void srand(unsigned int seed) {
  if (seed == 0) {
    state[0] = MAGIC3;
    state[1] = MAGIC4;
  } else {
    state[0] = seed + MAGIC1;
    state[1] = MAGIC2;
  }
}

int rand() {
  uint64_t x = state[0], y = state[1];
  state[0] = y;
  x ^= x << 23;
  state[1] = x ^ y ^ (x >> 17) ^ (y >> 26);
  return (int)(state[1] + y) & 0x7fffffff;
}

