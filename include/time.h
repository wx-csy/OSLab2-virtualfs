#ifndef _time_h
#define _time_h

#include <stdint.h>

typedef int64_t clock_t;
typedef int64_t time_t;

struct tm {
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};

#define CLOCKS_PER_SEC    1000

clock_t clock(void);
time_t time(time_t *arg);
char *ctime(const time_t *time);
char *asctime(const struct tm *time_ptr);
struct tm *localtime(const time_t *time);

#endif

