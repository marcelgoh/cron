all: test test_ndebug

INCDIRS = -I../src -I../include -I.
SOURCES = ../src/mgos_cron.c ../src/ccronexpr.c ../src/sunriset.c \
          ../src/common/cs_time.c ../src/common/cs_dbg.c \
          ../src/common/str_util.c ../src/common/mg_str.c \
          cs_dbg.c mgos_hal.c mgos_location.c mgos_timers.c cron_test_helpers.c \
          mgos_sys_config.c mgos_utils.c ../src/frozen/frozen.c
LIBS = -lm

test:
	gcc --std=c99 -D_POSIX_SOURCE -DCRON_USE_LOCAL_TIME=1 $(CFLAGS) $(INCDIRS) -g3 -m32 $(SOURCES) cron_test.c $(LIBS) -o cron_test
	./cron_test

test_ndebug:
	gcc --std=c99 -D_POSIX_SOURCE -DCRON_USE_LOCAL_TIME=1 $(CFLAGS) $(INCDIRS) -DNDEBUG -g3 -m32 $(SOURCES) cron_test.c $(LIBS) -o cron_test
	./cron_test

dst_new:
	cp ../src/ccronexpr_new.c ../src/ccronexpr.c
	cp ../src/ccronexpr_new.h ../src/ccronexpr.h
	cp ../src/mgos_cron_new.c ../src/mgos_cron.c
	gcc --std=c99 -D_POSIX_SOURCE -DCRON_USE_LOCAL_TIME=1 $(CFLAGS) $(INCDIRS) -g3 -m32 $(SOURCES) dst_test.c $(LIBS) -o dst_test
	./dst_test

dst_old:
	cp ../src/ccronexpr_old.c ../src/ccronexpr.c
	cp ../src/ccronexpr_old.h ../src/ccronexpr.h
	cp ../src/mgos_cron_old.c ../src/mgos_cron.c
	gcc --std=c99 -D_POSIX_SOURCE $(CFLAGS) $(INCDIRS) -D_REENT -g3 -m32 $(SOURCES) dst_test.c $(LIBS) -o dst_test
	./dst_test
