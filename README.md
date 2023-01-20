# Mongoose OS DST Cron tests

## Background

This is a fork of [mongoose-os-libs/cron](https://github.com/mongoose-os-libs/cron), modified to be a standalone repository for testing Mongoose's randomised cron functionality. In particular, all the imported dependencies are to be found in `src/common` and `src/frozen`. These files were taken à la carte from the most recent &mdash; as of 16 January 2023 &mdash; commits of [cesanta/mongoose](https://github.com/cesanta/mongoose), [cesanta/mongoose-os](https://github.com/cesanta/mongoose-os), and [cesanta/mjs](https://github.com/cesanta/mjs), and then `test/Makefile` was modified to point to the new locations of these files.

## Comparing two commits

In the `src` folder you'll find the following six files:

```
ccronexpr_old.c
ccronexpr_new.c
ccronexpr_old.h
ccronexpr_new.h
mgos_cron_old.c
mgos_cron_new.h
```

The files containing `_old` are taken from the [12 March 2022 version of `mongoose-os-libs/cron`](https://github.com/mongoose-os-libs/cron/tree/039f5a9f17ac67bc98791564b305e7016f99b884) and the files containing `_new` are taken from the [29 March 2022 version of the same repository](https://github.com/mongoose-os-libs/cron/tree/8c7f374b12a13365b9481fcc780872d7cdd2fdfc), which is the most recent one as of 20 January 2023.

## Running tests

The test code I've written is in `test/dst_test.c`. This file should be run by selecting one of two targets in `test/Makefile`. The `dst_old` target compiles the test code using the old versions of `src/ccronexpr.c`, `src/ccronexpr.h`, and `src/mgos_cron.c`, while the `dst_new` target compiles the project using the versions of these files from the most recent commit of `cron`.

## A test that differs between two commits

Because the tests concern the randomised cron functionality, you are encouraged to run `make dst_old` and/or `make dst_new` multiple times from the `test` directory to get a sense of what cron does. Here is an example of what `make dst_new` outputs:

```
NOW: 1647153020 (2022-03-12 23:30:20 UTC-7)
--------------------------------------------------
(TEST 0) Cron expression:
        @random:{"from":"0 0 0 * * *", "to":"59 59 23 * * *", "number":1}
Next invocation time:
        2022-03-13 20:22:46 UTC-6
--------------------------------------------------
(TEST 1) Cron expression:
        @random:{"from":"0 45 1 * * *", "to":"0 15 3 * * *", "number":1}
Next invocation time:
        2022-03-13 01:52:51 UTC-7
--------------------------------------------------
(TEST 2) Cron expression:
        @random:{"from":"0 59 1 * * *", "to":"0 1 3 * * *", "number":1}
Next invocation time:
        2022-03-13 03:00:41 UTC-6
--------------------------------------------------
(TEST 3) Cron expression:
        @random:{"from":"0 59 1 * * *", "to":"0 30 2 * * *", "number":1}
Next invocation time:
        2022-03-13 08:32:18 UTC-6
--------------------------------------------------
(TEST 4) Cron expression:
        @random:{"from":"0 30 2 * * *", "to":"0 15 3 * * *", "number":1}
Next invocation time:
        2022-03-13 01:55:28 UTC-7
--------------------------------------------------
(TEST 5) Cron expression:
        @random:{"from":"0 15 2 * * *", "to":"0 45 2 * * *", "number":1}
Next invocation time:
        2022-03-14 02:24:44 UTC-6
--------------------------------------------------
```

The time is set to 11:30:20 p.m. on 12 March 2022, a couple of hours before the switch to Daylight Savings Time (in Edmonton, Alberta). The test of note in this section is `TEST 1`, which asks for a random invocation time between 1:45 a.m. and 3:15 a.m. Because of the time change, this is not a 90-minute window; instead it is only 30 minutes. But running the test multiple times should convince you that the implementation is correct: i.e. a time of the form `1:XX:AA UTC-7` or `3:YY:BB UTC-6` is given, where `XX:AA` is between `45:00` and `59:59` and `YY:BB` is between `00:00` and `15:00`.

Now try running `make dst_old`. Depending on your luck, you may get something like this:

```
(TEST 1) Cron expression:
        @random:{"from":"0 45 1 * * *", "to":"0 15 3 * * *", "number":1}
Next invocation time:
        2022-03-14 02:15:04 UTC-6
```

This is not a valid time between the next 1:45 a.m.-to-3:45 a.m. window; indeed it is on 14 March, which is two days from `NOW`! Curiously, even if a time of the form `1:XX` or `3:YY` is returned, the date is still 14 March. My guess for why this might be is that the implementation somehow looks for the next _full_ 1:45-to-3:45 window and that is not till 14 March, since the hour between 2:00 and 2:59 on 13 March does not exist.

The same behaviour is shown in `TEST 2`, except the window is made even smaller (1:59 to 3:01) to further highlight the issue.

__Note.__ `make dst_old` produces a warning concerning an implicit declaration which can easily be fixed by adding `#include "common/cs_time.h"` to `src/mgos_cron.c`, but I thought it best to leave this file _exactly_ as I found it in the 12 March commit of the repo for experimentation's sake. Adding this line does not, as far as I can tell, fix any of the problems described above.

## Tests that "fail" in both commits

`TEST 3` and `TEST 4` are tests that could be said to fail in both commits, though this is a somewhat nuanced statement, since it depends on the how you interpret the semantics of a randomised cron expression. Consider the `TEST 3` expression

```
@random:{"from":"0 59 1 * * *", "to":"0 30 2 * * *", "number":1}
```

which in English means "schedule 1 invocation time between 1:59:00 and 2:30:00." Should one interpret this to mean

1. schedule an invocation time between _the next instance_ of 1:59 a.m. and _the next instance_ of 2:30 a.m.? or
2. schedule an invocation time between 1:59 a.m. and 2:30 a.m. _on the next day that both of these times appear_?

The `dst_old` code seems to do number 2, and pretty consistently, always returning times in the correct range on 14 March. The `dst_new` code seems to do `TEST 3` according to rule number 1, returning all sorts of times between 1:45 a.m. on 13 March and 2:30 a.m. on 14 March:

```
(TEST 3) Cron expression:
        @random:{"from":"0 59 1 * * *", "to":"0 30 2 * * *", "number":1}
Next invocation time:
        2022-03-13 22:25:29 UTC-6
```

But under the semantics of rule 1 above, for `TEST 4` the `dst_new` code should now look for times between 2:30 a.m. on 14 March and 3:15 a.m. on 13 March, which is a time interval of negative length! This causes `dst_new` to have a minor existential crisis, and in one iteration I found it to return a date that is incorrect under any reasonable interpretation of the given cron expression:

```
(TEST 4) Cron expression:
        @random:{"from":"0 30 2 * * *", "to":"0 15 3 * * *", "number":1}
Next invocation time:
        2022-03-13 01:54:33 UTC-7
```

__Note.__ There is no ambiguity in `TEST 5`, since under both interpretations, the next 2:15-to-2:45 window is on 14 March.

## Moral of the story

Based on the tests above, we can strongly suspect that:

1. When given a time window that fully contains the interval of time between 2:00 a.m and 2:59 a.m., `dst_new` works and `dst_old` is faulty during days where there is a forward time change.
2. When given a time window one endpoint of which is in the range 2:00--2:59, `dst_old` seems to work fine, provided we interpret a cron expression according to rule number 2 in the previous section, and `dst_new` produces what might euphemistically be called unspecified behaviour.