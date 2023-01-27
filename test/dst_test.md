# What happens in the hours before the spring time change


### Introduction

This short document explains what happens in Mongoose OS's implementation of cron _before_ the commit of March 29, 2022 by tracing through a specific example that causes an assert to be thrown. The assert is thrown in `mgos_cron.c` when `s_next_random()` is called with a `now` value that is later than the `from` value. The value of `from` is returned by the function `cron_next()` in `ccronexpr.c`, so let us dive into the computation of this value.

### Setting

It is 17:30 on March 13, 2021 and cron needs to schedule a job in the next 00:00-23:59 window. The next DST time change happens at 02:00 on March 14, 2021. The function `cron_next` wants to calculate the next `from` date, and since the expression for `from` is `"0 0 0 * * *`, the seconds already match. The minute doesn't match, and the next match is in the next hour, so the minute is set to `00` and the hour rolls forward to `18`.

### The hour computation in detail

We enter the [`find_next()` function](https://github.com/marcelgoh/cron/blob/f8bb8e63e1e72fc1a67191f7919e71349b90e94b/src/ccronexpr_old.c#L325) with `value = 18` and `next_value = 0`. We try to find `0` by incrementing the hour using `set_next_bit()`, but fail, so `notfound == 1` and we must enter the `if` statement to roll forward the date. At this point we have the following state of the variable `calendar`:

```
before add_to_field in find_next: field = 2, cal->tm_hour = 18, cal->tm_mday = 13, cal->tm_isdst = 0
```

So we call the [`add_to_field()` function](https://github.com/marcelgoh/cron/blob/f8bb8e63e1e72fc1a67191f7919e71349b90e94b/src/ccronexpr_old.c#L197) with `field == nextField`, the latter of which happens to be `3`, indicating that we should increment the day. This function does its thing, and we see that the `calendar`'s hour is now incremented by one (`13` changed to `14` in `cal->tm_mday` but nothing else changed).

```
before cron_mktime in add_to_field: field = 3, cal->tm_hour = 18, cal->tm_mday = 14, cal->tm_isdst = 0
```

Then we do something seemingly innocuous: we call `time_t res = cron_mktime(calendar);` and check whether `res` is an error value. But [the `cron_mktime` function](https://github.com/marcelgoh/cron/blob/f8bb8e63e1e72fc1a67191f7919e71349b90e94b/src/ccronexpr_old.c#L132), which just calls the system's `mktime` function, doesn't just get the Unix epoch of a `struct tm`, it also _modifies_ the input struct to make sure things make sense according to local time. Here `mktime` sees that `tm_isdst` is set to false, but detects that DST should be in effect, so it sets this value to true and then guesses that the time is not actually 18:00 but 19:00! Thus after `cron_mktime` is called, the calendar looks like this:

```
after cron_mktime in add_to_field: field = 3, cal->tm_hour = 19, cal->tm_mday = 14, cal->tm_isdst = 1
```

Now that we've incremented the hour, we need to set the `tm_hour` from whatever it is to zero, because midnight on March 14 is the correct `from` date. This is done by the [function `reset()`](https://github.com/marcelgoh/cron/blob/f8bb8e63e1e72fc1a67191f7919e71349b90e94b/src/ccronexpr_old.c#L234). But this function also calls `time_t res = cron_mktime(calendar);` to ensure it hasn't caused an error. And now `cron_mktime` sees that `tm_hour == 0`, `tm_mday == 14`, but `tm_isdst == 1`! Because this is before 02:00, it notes that `tm_isdst` should be `0`, but then assumes that since it was erroneously set to `1`, the real time must be 23:00 on March 13th! So we have

```
after reset in find_next: field = 2, cal->tm_hour = 23, cal->tm_mday = 13, cal->tm_isdst = 0
```

and the day didn't get incremented at all. But the madness doesn't stop there. Because we've exited the first `if` block without successfully setting `value` equal to `new_value`, we enter the next `if` statement as well (which we weren't supposed to). This calls [yet another resetting operation](https://github.com/marcelgoh/cron/blob/f8bb8e63e1e72fc1a67191f7919e71349b90e94b/src/ccronexpr_old.c#L270), this time called `reset_all`, that sets all the lower-order values in the `calendar`, namely, the hour, minute, and second. After this unfortunate function call, the calendar values look like

```
after reset_all in find_next: field = 2, cal->tm_hour = 0, cal->tm_mday = 13, cal->tm_isdst = 0
```

So we end up with a `from` date of 00:00 on March 13, 2021, which is actually _before_ the current time.

### Why this doesn't happen in the new code

The [new version of `cron_mktime()`](https://github.com/marcelgoh/cron/blob/f8bb8e63e1e72fc1a67191f7919e71349b90e94b/src/ccronexpr_new.c#L182) calls [another function called `cron_mktime_local()`](https://github.com/marcelgoh/cron/blob/f8bb8e63e1e72fc1a67191f7919e71349b90e94b/src/ccronexpr_new.c#L153), which sets the calendar's `tm_isdst` field to `-1` before calling the system's `mktime()` function. This essentially tells `mktime` that we have messed with timestamps and _don't know_ whether DST is in effect or not. Then `mktime` determines the correct DST flag without further altering the timestamp.

