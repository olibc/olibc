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
#include <sys/ioctl.h>
#include <pty.h>

int login_tty(int fd)
{
  setsid();
  if (ioctl(fd, TIOCSCTTY, (char *)NULL) == -1) {
    return -1;
  }

  dup2(fd, 0);
  dup2(fd, 1);
  dup2(fd, 2);
  if (fd > 2) {
    close (fd);
  }
  return 0;
}
