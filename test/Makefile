all: test test_ndebug

INCDIRS = -I../src -I../include -I.
SOURCES = ../src/mgos_cron.c ../src/ccronexpr.c ../src/sunriset.c \
          ../src/common/cs_time.c \
          ../src/common/str_util.c ../src/common/mg_str.c \
          cs_dbg.c mgos_hal.c mgos_location.c mgos_timers.c \
          mgos_sys_config.c mgos_utils.c ../src/frozen/frozen.c cron_test.c
LIBS = -lm

test:
	gcc --std=c99 -D_POSIX_SOURCE -DCRON_USE_LOCAL_TIME=1 $(CFLAGS) $(INCDIRS) -g3 -m32 $(SOURCES) $(LIBS) -o cron_test
	./cron_test

test_ndebug:
	gcc --std=c99 -D_POSIX_SOURCE -DCRON_USE_LOCAL_TIME=1 $(CFLAGS) $(INCDIRS) -DNDEBUG -g3 -m32 $(SOURCES) $(LIBS) -o cron_test
	./cron_test
