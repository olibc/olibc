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
#include "passwd_private.h"

int putspent (const struct spwd *p, FILE *fp)
{
  if (fp == NULL || p == NULL) {
    return -1;
  } else {
    fprintf(fp, "%s:%s:",
                p->sp_namp,
                (p->sp_pwdp ? p->sp_pwdp : ""));
    const long spwdentry[6] = {p->sp_lstchg, p->sp_min, p->sp_max,
                               p->sp_warn, p->sp_inact, p->sp_expire};
    size_t i;
    for (i = 0;i<sizeof(spwdentry)/sizeof(long);++i) {
      if (spwdentry[i] != -1) {
        fprintf(fp, "%ld", spwdentry[i]);
      }
      fputc(':', fp);
    }
    if (p->sp_flag != ~0UL) {
      fprintf(fp, "%lu", p->sp_flag);
    }
    fputc('\n', fp);
    return 0;
  }
}
