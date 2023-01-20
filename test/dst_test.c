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

void cb(void *user_data, mgos_cron_id_t id) {
  mgos_cron_remove(id);
  printf("\n--------------------------------------------------\n");
}

int main(void) {
  char s_tz[8] = {0};
  char s_str_time[32] = {0};
  struct tm s_tm = {0};
  char test_expr[256];
  char *exprs[] = {
    "@random:{\"from\":\"0 0 0 * * *\", \"to\":\"59 59 23 * * *\", \"number\":1}",
    "@random:{\"from\":\"0 45 1 * * *\", \"to\":\"0 15 3 * * *\", \"number\":1}",
    "@random:{\"from\":\"0 59 1 * * *\", \"to\":\"0 1 3 * * *\", \"number\":1}",
    "@random:{\"from\":\"0 59 1 * * *\", \"to\":\"0 30 2 * * *\", \"number\":1}",
    "@random:{\"from\":\"0 30 2 * * *\", \"to\":\"0 15 3 * * *\", \"number\":1}",
    "@random:{\"from\":\"0 15 2 * * *\", \"to\":\"0 45 2 * * *\", \"number\":1}"
  };
  int exprs_sz = ARRAY_SIZE(exprs);
  sprintf(test_expr, "@random:{\"from\":\"0 0 0 * * *\", \"to\":\"59 59 23 * * *\", \"number\":1}");
  void *user_data;

  srand(time(NULL));
  s_get_timezone(s_tz);
  time_t t = s_cron_test_str2timeloc(&s_tm, TEST_BASE_TIME);
  mgos_set_time(t);
  time_t ct = (time_t) mg_time();

  printf(
      "\nNOW: %lu (%s)\n"
      "--------------------------------------------------\n",
      ct, s_cron_test_timeloc2str(s_str_time, ARRAY_SIZE(s_str_time), ct, s_tz));

  for (int i = 0; i < exprs_sz; ++i) {
    strcpy(test_expr, exprs[i]);
    printf("(TEST %d) Cron expression:\n\t%s\n", i, test_expr);
    mgos_cron_id_t id = mgos_cron_add(test_expr, cb, user_data);
    time_t ni = mgos_cron_get_next_invocation(id, ct);
    printf("Next invocation time:\n\t%s\n", s_cron_test_timeloc2str(s_str_time, ARRAY_SIZE(s_str_time), ni, s_tz));
    printf("--------------------------------------------------\n");
  }
}
