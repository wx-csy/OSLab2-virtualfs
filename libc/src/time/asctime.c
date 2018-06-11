#include <time.h>
#include <stdio.h>

char* asctime(const struct tm *time_ptr) {
  static const char wday_name[][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
  static const char mon_name[][4] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  static char result[32];
  sprintf(result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
      wday_name[time_ptr->tm_wday], mon_name[time_ptr->tm_mon],
      time_ptr->tm_mday, time_ptr->tm_hour, time_ptr->tm_min, 
      time_ptr->tm_sec, 1900 + time_ptr->tm_year);
  return result;
}

