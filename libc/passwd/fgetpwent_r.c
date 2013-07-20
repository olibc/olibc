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

#include <pwd.h>
#include <stdlib.h>
#include "passwd_private.h"

int fgetpwent_r(FILE *fp, struct passwd *pwbuf, char *buf,
                size_t buflen, struct passwd **pwbufp)
{
  *pwbufp = NULL;
  while (fgets(buf, buflen, fp)) {
    int pos[6];
    char *str = buf;
    int idx = 0;
    while (*str) {
      if (*str == ':' && idx < 6) {
        pos[idx++] = str - buf + 1;
        *str = '\0';
      }
      ++str;
    }
    if (idx != 6) {
      continue;
    }
    if (str[-1] == '\n') {
      str[-1] = '\0';
    }

    pwbuf->pw_name = buf;
    pwbuf->pw_passwd = buf + pos[0];
    pwbuf->pw_uid = atoi(buf + pos[1]);
    pwbuf->pw_gid = atoi(buf + pos[2]);
    pwbuf->pw_gecos = buf + pos[3];
    pwbuf->pw_dir = buf + pos[4];
    pwbuf->pw_shell = buf + pos[5];
    *pwbufp = pwbuf;
    return 0;
  }
  return -1;
}
