/*
 * Copyright (c) 2014-2018 Cesanta Software Limited
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cron_test_helpers.h"
#include "common/platform.h"

#define TEST_BASE_TIME "2017-08-29 22:00:20"

extern long timezone;

static char s_tz[8] = {0};
static char s_str_time[32] = {0};
static struct tm s_tm = {0};

int main(void) {
  s_get_timezone(timezone, s_tz);
  printf("%s\n", s_tz);
  size_t sz = ARRAY_SIZE(s_str_time);
  printf("This is a set of tests for Daylight Savings Time.\n");
  time_t t = s_cron_test_str2timeloc(&s_tm, TEST_BASE_TIME);
  printf(
      "\nNOW: %lu (%s)\n"
      "--------------------------------------------------\n",
      t, s_cron_test_timeloc2str(s_str_time, sz, t, s_tz));

  return 0;
}