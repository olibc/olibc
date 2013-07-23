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

#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pty.h>
#include <utmp.h>

pid_t forkpty(int *amaster, char *name,
              const struct termios *termp,
              const struct winsize *winp)
{
  int master, slave, pid;
  if (openpty (&master, &slave, name, termp, winp) == -1) {
    return -1;
  }
  pid = fork();
  if (pid <= 0) {
    close(master);
    if (pid == 0) {
      if (login_tty (slave)) {
        exit(1);
      }
      return 0;
    } else {
      close (slave);
      return -1;
    }
  } else {
    *amaster = master;
    close(slave);
    return pid;
  }
}
