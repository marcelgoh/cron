#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/cs_time.h"
#include "mgos_cron.h"
#include "mgos_timers.h"
#include "mgos_location.h"

void s_get_timezone(char s_tz[]);

struct tm *s_cron_test_str2tm(struct tm *s_tm, const char *date);

time_t s_cron_test_str2timeloc(struct tm *s_tm, const char *date);

time_t s_cron_test_str2timegm(struct tm *s_tm, const char *date);

char *s_cron_test_timeloc2str(char s_str_time[], size_t sz, time_t date, char s_tz[]);

char *s_cron_test_timegm2str(char s_str_time[], size_t sz, time_t date);
