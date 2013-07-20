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
#include <string.h>
#include "passwd_private.h"

static int spwd_find_by_name(void *spwd,
                             const void *spwd_target)
{
  struct spwd *_spwd = (struct spwd *)spwd;
  const char *_spwd_target = (const char *)spwd_target;
  return strcmp(_spwd->sp_namp, _spwd_target) == 0;
}

static int spwd_next(void *spbuf, char *buf, size_t buflen, void **spbufp)
{
  struct spwd *_spbuf = (struct spwd *)spbuf;
  struct spwd **_spbufp = (struct spwd **)spbufp;
  return getspent_r(_spbuf, buf,
                    buflen, _spbufp);
}


int getspnam_r(const char *name, struct spwd *spbuf,
               char *buf, size_t buflen, struct spwd **spbufp)
{
  return __pwdgrp_find((void*)name, (void*)spbuf,
                       buf, buflen, (void**)spbufp,
                       setspent, endspent,
                       spwd_next, spwd_find_by_name);
}
