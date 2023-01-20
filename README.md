# Mongoose OS Cron tests

## Background

This is a fork of [mongoose-os-libs/cron](https://github.com/mongoose-os-libs/cron), modified to be a standalone repository for testing Mongoose's randomised cron functionality. In particular, all the new code is to be found in `src/common`, `src/frozen`. These files were taken à la carte from the most recent &mdash; as of 16 January 2023 &mdash; commits of [cesanta/mongoose](https://github.com/cesanta/mongoose), [cesanta/mongoose-os](https://github.com/cesanta/mongoose-os), and [cesanta/mjs](https://github.com/cesanta/mjs), and then `test/Makefile` was modified to point to the new locations of these files.

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

The files containing `_old` are taken from the [12 March 2022 version of `mongoose-os-libs/cron`](https://github.com/mongoose-os-libs/cron/tree/039f5a9f17ac67bc98791564b305e7016f99b884) and the files containing `new` are taken from the [29 March 2022 version of the same repository](https://github.com/mongoose-os-libs/cron/tree/8c7f374b12a13365b9481fcc780872d7cdd2fdfc), which is the most recent one as of 20 January 2023.

## Running tests

The test code I've written is in `test/dst_test.c`. This file should be run by selecting one of two targets in `test/Makefile`, there are two 