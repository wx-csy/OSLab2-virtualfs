#include <time.h>

static inline int get_wday(int year, int month, int day) {
  month++;
  if (month <= 2) {
    month += 12;
    year--;
  }
  int totday = 1 + day + 2 * month + 3 * (month + 1) / 5;
  totday += year + year / 4 - year / 100 + year / 400;
  return totday % 7; 
}

static inline int get_yday(int year, int month, int day) {
  const int off[] = {0, 31, 59, 90, 120, 151, 
    181, 212, 243, 273, 304, 334};
  int yday = off[month] + day;
  int is_leap = (year % 4 == 0 && 
      (year % 100 != 0 || year % 400 == 0));
  if (month >= 2 && is_leap) yday++;
  return yday;
}

#define MASK(x) ((1 << (x)) - 1)

struct tm *localtime(const time_t *time) {
  static struct tm tm_buf;
  time_t val = *time;
  tm_buf.tm_sec = val & MASK(6);  val >>= 6;
  tm_buf.tm_min = val & MASK(6);  val >>= 6;
  tm_buf.tm_hour = val & MASK(5); val >>= 5;
  tm_buf.tm_mday = val & MASK(5); val >>= 5;
  tm_buf.tm_mon = (val & MASK(4)) - 1;  val >>= 4;
  tm_buf.tm_year = val - 1900;
  tm_buf.tm_wday = get_wday(val, tm_buf.tm_mon, tm_buf.tm_mday);
  tm_buf.tm_yday = get_yday(val, tm_buf.tm_mon, tm_buf.tm_mday);
  tm_buf.tm_isdst = -1;
  return &tm_buf;
}

