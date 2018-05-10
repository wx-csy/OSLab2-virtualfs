#include <time.h>

char *ctime(const time_t *time) {
  return asctime(localtime(time));
}

