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


#include <unistd.h>
#include <errno.h>

/* Magic number come from sysconf.c's SYSTEM_TTY_NAME_MAX. */
#define MAX_TTY_NAME_LEN 32

char *ttyname(int fd)
{
  static char buf[MAX_TTY_NAME_LEN];
  int rv;
  rv = ttyname_r(fd, buf, sizeof(buf));
  if (rv == 0) {
    return buf;
  } else {
    errno = rv;
    return NULL;
  }
}
