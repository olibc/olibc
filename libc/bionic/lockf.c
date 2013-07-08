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

#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include "utimes_internal.h"

int lockf(int fd, int cmd, off_t len)
{
  struct flock fl;
  fl.l_whence = SEEK_CUR;
  fl.l_start = 0;
  fl.l_len = len;
  if (cmd < 0 /* F_ULOCK */ && cmd > 3 /* F_TEST */) {
    errno = EINVAL;
    return -1;
  }
  const int action[4][2] =
  {
    [F_TEST]  = {F_RDLCK, F_GETLK},
    [F_ULOCK] = {F_UNLCK, F_SETLK},
    [F_LOCK]  = {F_WRLCK, F_SETLKW},
    [F_TLOCK] = {F_WRLCK, F_SETLK}
  };
  int fcntlcmd = action[cmd][1];
  fl.l_type = action[cmd][0];
  int rv = fcntl(fd, fcntlcmd, &fl);
  if (cmd == F_TEST) {
    if (rv < 0) {
      return -1;
    }
    if (fl.l_type == F_UNLCK ||
        fl.l_pid == getpid ()) {
      return 0;
    }
    errno = EACCES;
    return -1;
  } else {
    return rv;
  }
}
