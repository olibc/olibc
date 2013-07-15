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

#include <shadow.h>
#include <stdio.h>
#include <stdlib.h>
#include "passwd_private.h"

static long parse_spent_num(char *str)
{
  if (str[0] == 0 || str[0] == '\n') {
    return -1;
  } else {
    return atol(str);
  }
}

int fgetspent_r(FILE *fp, struct spwd *spbuf,
                char *buf, size_t buflen,
                struct spwd **spbufp)
{
  *spbufp = NULL;

  while (fgets(buf, buflen, fp)) {
    int pos[8];
    int idx = 0;
    char *str = buf;
    while (*str) {
      if (*str == ':') {
        pos[idx++] = str - buf + 1;
        *str = '\0';
      }
      ++str;
    }
    if (idx != 8) {
      continue;
    }
    spbuf->sp_namp = buf;
    spbuf->sp_pwdp = buf + pos[0];
    spbuf->sp_lstchg = parse_spent_num(buf + pos[1]);
    spbuf->sp_min = parse_spent_num(buf + pos[2]);
    spbuf->sp_max = parse_spent_num(buf + pos[3]);
    spbuf->sp_warn = parse_spent_num(buf + pos[4]);
    spbuf->sp_inact = parse_spent_num(buf + pos[5]);
    spbuf->sp_expire = parse_spent_num(buf + pos[6]);
    spbuf->sp_flag = parse_spent_num(buf + pos[7]);
    *spbufp = spbuf;
    return 0;
  }
  return -1;
}
