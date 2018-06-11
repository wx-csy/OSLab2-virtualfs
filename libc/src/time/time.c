#include <time.h>
#include <stddef.h>
#include <am.h>
#include <amdev.h>

static _Device *dev_timer = NULL;
static int initialized;

static inline void initialize() {
  if (initialized) return;
  for (int n = 1; ; n++) {
    _Device *dev = _device(n);
    if (!dev) break;
    if (dev->id == _DEV_TIMER) {
      dev_timer = dev;
      break;
    }
  }
  initialized = 1;
}

time_t time(time_t *arg) {
  initialize();
  if (dev_timer == NULL) {
    if (arg != NULL) *arg = -1;
    return -1;
  } else {
    _RTCReg rtctime;
    dev_timer->read(_DEVREG_TIMER_DATE, &rtctime, sizeof rtctime);
    time_t res;
    res = rtctime.year;
    res = res << 4 | rtctime.month;
    res = res << 5 | rtctime.day;
    res = res << 5 | rtctime.hour;
    res = res << 6 | rtctime.minute;
    res = res << 6 | rtctime.second;
    if (arg != NULL) *arg = res;
    return res;
  }
}

clock_t clock(void) {
  initialize();
  if (dev_timer == NULL) {
    return -1;
  } else {
    _UptimeReg uptime;
    dev_timer->read(_DEVREG_TIMER_UPTIME, &uptime, sizeof uptime);
    return (clock_t)uptime.hi << 32 | (clock_t)uptime.lo;
  }
}

