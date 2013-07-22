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
#include <stdio.h>

#define FD_PATH "/proc/self/fd/"
#define FD_PATH_TEMPLATE FD_PATH "%d"
#define MAX_TTY_NAME_LEN (sizeof(FD_PATH) + sizeof(int) * 4)

int ttyname_r(int fd, char *buf, size_t buflen)
{
  char tty_fd_path_buf[MAX_TTY_NAME_LEN];
  ssize_t linklen;
  snprintf(tty_fd_path_buf, MAX_TTY_NAME_LEN, FD_PATH_TEMPLATE, fd);

  /* fd is not tty.*/
  if (!isatty(fd)) {
    return ENOTTY;
  }

  linklen = readlink(tty_fd_path_buf, buf, buflen);

  if (linklen < 0) {
    return ENOTTY;
  } else if (linklen == (ssize_t)buflen) {
    /* buffer is not large enough. */
    return ERANGE;
  } else {
    buf[linklen] = '\0';
    return 0;
  }
}
