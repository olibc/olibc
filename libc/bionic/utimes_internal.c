/*
 * Copyright (C) 2013 olibc developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "utimes_internal.h"

#include <errno.h>
#include <sys/time.h>
#include <stdbool.h>
#include <sys/stat.h>

static bool valid(const struct timeval *tv)
{
  return tv->tv_usec >= 0 && tv->tv_usec < 1000000;
}

static void timeval_to_timespec(const struct timeval *tv, struct timespec *ts)
{
  ts->tv_sec  = tv->tv_sec;
  ts->tv_nsec = tv->tv_usec * 1000;
}

int utimes_internal(int fd,
                    const char *path,
                    int flags,
                    const struct timeval tv[2])
{
  struct timespec ts[2];
  if (valid(&tv[0]) && valid(&tv[1])) {
    timeval_to_timespec(&tv[0], &ts[0]);
    timeval_to_timespec(&tv[1], &ts[1]);
    return utimensat(fd, path, ts, flags);
  } else {
    errno = EINVAL;
    return -1;
  }
}
