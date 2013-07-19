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

struct spwd_next_func_arg_t {
  struct spwd *spbuf;
  char *buf;
  size_t buflen;
  struct spwd **spbufp;
};

static int spwd_find_by_name(void *spwd,
                             const void *spwd_target)
{
  struct spwd *_spwd = (struct spwd *)spwd;
  const char *_spwd_target = (const char *)spwd_target;
  return strcmp(_spwd->sp_namp, _spwd_target) == 0;
}

static void *spwd_next(void *arg)
{
  struct spwd_next_func_arg_t *spwd_arg = (struct spwd_next_func_arg_t*)arg;
  getspent_r(spwd_arg->spbuf, spwd_arg->buf,
             spwd_arg->buflen, spwd_arg->spbufp);
  return (void*)(*spwd_arg->spbufp);
}


int getspnam_r(const char *name, struct spwd *spbuf,
               char *buf, size_t buflen, struct spwd **spbufp)
{
  const char *spwd_target = name;
  struct spwd_next_func_arg_t spwd_next_func_arg;
  spwd_next_func_arg.spbuf = spbuf;
  spwd_next_func_arg.buf = buf;
  spwd_next_func_arg.buflen = buflen;
  spwd_next_func_arg.spbufp = spbufp;
  return __pwdgrp_find(setspent, endspent,
                       spwd_next, &spwd_next_func_arg,
                       spwd_find_by_name, spwd_target,
                       (void**)spbufp);
}
