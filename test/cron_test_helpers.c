#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/cs_time.h"
#include "mgos_cron.h"
#include "mgos_timers.h"
#include "mgos_location.h"

extern long timezone;

void s_get_timezone(char s_tz[]) {
  tzset();
  int t = timezone / 60 / 60;
  if (t == 0) {
    strcpy(s_tz, "UTC");
  } else {
    bool neg = false;
    if (t < 0) {
      neg = true;
      t *= -1;
    }
    sprintf(s_tz, "UTC%c%d", neg ? '+' : '-', t);
  }
}

struct tm *s_cron_test_str2tm(struct tm *s_tm, const char *date) {
  memset(s_tm, 0, sizeof(*s_tm));

  sscanf(date, "%04d-%02d-%02d %02d:%02d:%02d", &(s_tm->tm_year), &(s_tm->tm_mon),
         &(s_tm->tm_mday), &(s_tm->tm_hour), &(s_tm->tm_min), &(s_tm->tm_sec));
  s_tm->tm_mon -= 1;
  s_tm->tm_year -= 1900;
  s_tm->tm_isdst = -1;

  return s_tm;
}

time_t s_cron_test_str2timeloc(struct tm *s_tm, const char *date) {
  return mktime(s_cron_test_str2tm(s_tm, date));
}

time_t s_cron_test_str2timegm(struct tm *s_tm, const char *date) {
  return (time_t) cs_timegm(s_cron_test_str2tm(s_tm, date));
}

char *s_cron_test_tm2str(char s_str_time[], size_t sz, struct tm *t, const char *gmt) {
  memset(s_str_time, 0, sz);
  snprintf(s_str_time, sz - 1, "%04d-%02d-%02d %02d:%02d:%02d %s",
           t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min,
           t->tm_sec, gmt);
  return s_str_time;
}

char *s_cron_test_timeloc2str(char s_str_time[], size_t sz, time_t date, char s_tz[]) {
  struct tm t;
  localtime_r(&date, &t);
  return s_cron_test_tm2str(s_str_time, sz, &t, s_tz);
}

char *s_cron_test_timegm2str(char s_str_time[], size_t sz, time_t date) {
  struct tm t;
  gmtime_r(&date, &t);
  return s_cron_test_tm2str(s_str_time, sz, &t, "UTC");
}

