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
#include "common/cs_dbg.h"

#define CS_ENABLE_STDIO 1
#define _REENT 1

#define TEST_BASE_TIME "2022-03-12 23:30:20"  //a couple of hours before time-change to DST

void cb(void *user_data, mgos_cron_id_t id) {
  mgos_cron_remove(id);
  printf("\n--------------------------------------------------\n");
}

int main(void) {
  cs_log_set_level(LL_VERBOSE_DEBUG);
  //cs_log_set_level(LL_NONE);
  FILE *err_file = fopen("error_log.txt", "w");
  cs_log_set_file(err_file);
  char s_tz[8] = {0};
  char s_str_time[32] = {0};
  struct tm s_tm = {0};
  char test_expr[256];
  char *base_times[] = {
    "2021-03-13 17:30:00",
    "2022-03-12 17:30:00",
    "2023-03-11 17:30:00"
  };
  char *exprs[] = {
    "@random:{\"from\":\"0 0 0 * * *\", \"to\":\"59 59 23 * * *\", \"number\":1}",
    //"@random:{\"from\":\"0 45 1 * * *\", \"to\":\"0 15 3 * * *\", \"number\":1}",
    //"@random:{\"from\":\"0 59 1 * * *\", \"to\":\"0 1 3 * * *\", \"number\":1}",
    //"@random:{\"from\":\"0 59 1 * * *\", \"to\":\"0 30 2 * * *\", \"number\":1}",
    //"@random:{\"from\":\"0 30 2 * * *\", \"to\":\"0 15 3 * * *\", \"number\":1}",
    //"@random:{\"from\":\"0 15 2 * * *\", \"to\":\"0 45 2 * * *\", \"number\":1}"
    //"@random:{\"from\":\"0 0 0 * * *\", \"to\":\"59 59 23 * * *\", \"number\":2}",
    //"@random:{\"from\":\"0 0 0 * * *\", \"to\":\"59 59 23 * * *\", \"number\":4}",
    //"@random:{\"from\":\"0 0 0 * * *\", \"to\":\"59 59 23 * * *\", \"number\":12}",
  };
  int num_invocations_to_test = 1;
  int exprs_sz = ARRAY_SIZE(exprs);
  sprintf(test_expr, "@random:{\"from\":\"0 0 0 * * *\", \"to\":\"59 59 23 * * *\", \"number\":1}");
  void *user_data;

  srand(time(NULL));
  s_get_timezone(s_tz);
  time_t t, ct;

  for (int year = 0; year < 1; ++year) {
    t = s_cron_test_str2timeloc(&s_tm, base_times[year]);
    mgos_set_time(t);
    ct = (time_t) mg_time();
    printf(
        "\nNOW: %lu (%s)\n"
        "--------------------------------------------------\n",
        ct, s_cron_test_timeloc2str(s_str_time, ARRAY_SIZE(s_str_time), ct, s_tz));
    for (int i = 0; i < exprs_sz; ++i) {
      mgos_set_time(t); // set time back to TEST_BASE_TIME
      ct = (time_t) mg_time();
      strcpy(test_expr, exprs[i]);
      printf("in main(): mg_time() = %f\n", mg_time());
      mgos_cron_id_t id = mgos_cron_add(test_expr, cb, user_data);
      printf("back in main(): mg_time() = %f\n", mg_time());
      printf("(TEST %d -- id=%u) Cron expression:\n\t%s\n", i, id, test_expr);
      time_t ni = 0;
      for (int j = 0; j < num_invocations_to_test; ++j) {
        ni = mgos_cron_get_next_invocation(id, ct);
        printf("Invocation %d:\n\t%lu (%s)\n", j, ni,
                s_cron_test_timeloc2str(s_str_time, ARRAY_SIZE(s_str_time), ni, s_tz));
        mgos_set_time(ni);
        ct = (time_t) mg_time();
      }
      printf("--------------------------------------------------\n");
    }
    printf("==================================================\n");
  }
  fclose(err_file);
};
