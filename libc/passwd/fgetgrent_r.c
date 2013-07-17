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

#include <grp.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int fgetgrent_r(FILE *fp, struct group *gbuf, char *buf,
                size_t buflen, struct group **gbufp)
{
  *gbufp = NULL;
  while (fgets(buf, buflen, fp)) {
    int pos[3];
    char *str = buf;
    int idx = 0;
    while (*str) {
      if (*str == ':' && idx < 3) {
        pos[idx++] = str - buf + 1;
        *str = '\0';
      }
      ++str;
    }
    if (idx != 3) {
      continue;
    }
    if (str[-1] == '\n') {
      str[-1] = '\0';
    }

    gbuf->gr_name = buf;
    gbuf->gr_passwd = buf + pos[0];
    gbuf->gr_gid = atoi(buf + pos[1]);

    const uintptr_t ptr_size = sizeof(void*);
    const uintptr_t mask = ~(ptr_size - 1);
    /* Assumes pointer size is a power of 2. */
    char **member_buffer = (char **)((uintptr_t)((str+1) + (ptr_size-1)) &
                                     mask);
    char *mem_buf = buf + pos[2];
    str = mem_buf;
    char *member = mem_buf;
    gbuf->gr_mem = member_buffer;
    if (*str) {
      while (*str) {
        if (*str == ',') {
          if ((uintptr_t)member_buffer > (uintptr_t)(buf + buflen)) {
            /* Oops! run out of buffer. */
            return -1;
          }
          *member_buffer++ = member;
          *str = '\0';
          member = str + 1;
        }
        ++str;
      }
      *member_buffer++ = member;
      *member_buffer = NULL;
    } else {
      /* Nobody in this group. */
      gbuf->gr_mem = NULL;
    }
    *gbufp = gbuf;
    return 0;
  }
  return -1;
}
