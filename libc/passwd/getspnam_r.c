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
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <paths.h>
#include "passwd_private.h"

int getspnam_r(const char *name, struct spwd *spbuf,
               char *buf, size_t buflen, struct spwd **spbufp)
{
  int retval;
  setspent();
  while (!(retval = getspent_r(spbuf, buf, buflen, spbufp))) {
    if (strcmp(spbuf->sp_namp, name) == 0) {
      return 0;
    }
  }
  if (retval) {
    /* Something wrong in getspent_r */
    return retval;
  } else {
    /* Just not found */
    return ERANGE;
  }
}
