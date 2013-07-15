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
#include <errno.h>
#include <paths.h>
#include "passwd_private.h"

static FILE *spf = NULL;

void setspent() {
  if (spf) {
    rewind(spf);
  }
}

void endspent() {
  if (spf) {
    fclose(spf);
    spf = NULL;
  }
}

int getspent_r(struct spwd *spbuf,
               char *buf, size_t buflen,
               struct spwd **spbufp)
{
  if (spf == NULL) {
    spf = fopen(_PATH_SHADOW, "re");
    if (spf == NULL) {
      return errno;
    }
  }
  return fgetspent_r(spf, spbuf, buf, buflen, spbufp);
}
