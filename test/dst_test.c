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

#include "common/cs_time.h"
#include "mgos_cron.h"
#include "cron_test_helpers.h"

#define TEST_BASE_TIME "2022-03-12 23:30:20"  //a couple of hours before time-change to DST

int main(void) {
  char s_tz[8] = {0};
  char s_str_time[32] = {0};
  struct tm s_tm = {0};

  srand(time(NULL));
  s_get_timezone(s_tz);
  time_t t = s_cron_test_str2timeloc(&s_tm, TEST_BASE_TIME);
  mgos_set_time(t);
  time_t ct = (time_t) mg_time();

  printf(
      "\nNOW: %lu (%s)\n"
      "--------------------------------------------------\n",
      ct, s_cron_test_timeloc2str(s_str_time, ARRAY_SIZE(s_str_time), ct, s_tz));

}
