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

#include "passwd_private.h"

char __spwd_buf[SPWD_BUFFER_SIZE];
struct spwd __spwd_spwdbuf;

char __pwd_buf[PWD_BUFFER_SIZE];
struct passwd __pwd_pwdbuf;

char grp_buf[GRP_BUFFER_SIZE];
struct group grp_gbuf;

int __pwdgrp_find(const void *target,
                  void *pwdgrpbuf,
                  char *buf,
                  size_t buflen,
                  void *pwdgrpbufp,
                  pwdgrp_init_func_t init_func,
                  pwdgrp_finit_func_t finit_func,
                  pwdgrp_next_func_t next_func,
                  pwdgrp_compare_func_t compare_func)
{
  int found = 0;
  init_func();
  while (next_func(pwdgrpbuf, buf, buflen, pwdgrpbufp)) {
    if (compare_func(pwdgrpbuf, target)) {
      found = 1;
      break;
    }
  }
  finit_func();
  if (found) {
    return 0;
  } else {
    *((char**)pwdgrpbufp) = NULL;
    return -1;
  }
}

int grp_next(void *grp,
             char *buf,
             size_t buflen,
             void **result)
{
  struct group *_grp = (struct group *)grp;
  struct group **_result = (struct group **)result;
  return getgrent_r(_grp, buf,
                    buflen, _result);
}

int pwd_next(void *pwd,
             char *buf,
             size_t buflen,
             void **result)
{
  struct passwd *_pwd = (struct passwd*)pwd;
  struct passwd **_result = (struct passwd**)result;
  return getpwent_r(_pwd, buf,
                    buflen, _result);
}
