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

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pty.h>

/* Magic number come from ptsname.c */
#define MAX_PTY_NAME_LEN 64

int openpty(int *amaster, int *aslave, char *name,
            const struct termios *termp, const struct winsize *winp)
{
  int master = *amaster;
  int slave;
  char ptyname[MAX_PTY_NAME_LEN];
  int unlock = 0;

  int fd = open("/dev/ptmx", O_RDWR);
  if (fd < 0) {
    goto error;
  }

  if (ioctl (fd, TIOCSPTLCK, &unlock) != 0) {
    goto error;
  }

  if (ptsname_r(fd, ptyname, MAX_PTY_NAME_LEN) != 0) {
    goto error;
  }

  slave = open(ptyname, O_RDWR|O_NOCTTY);
  if (slave < 0) {
    goto error;
  }

  *amaster = master;
  *aslave = slave;
  if (name != NULL) {
    strcpy(name, ptyname);
  }

  if (termp) {
    TEMP_FAILURE_RETRY(tcsetattr(master, TCSANOW, termp));
  }

  if (winp) {
    TEMP_FAILURE_RETRY(ioctl(master, TIOCSWINSZ, winp));
  }

error:
  close(fd);
  return -1;
}
