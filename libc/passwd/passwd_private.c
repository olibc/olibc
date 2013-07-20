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

int __pwdgrp_find(pwdgrp_init_func_t init_func,
                  pwdgrp_finit_func_t finit_func,
                  pwdgrp_next_func_t next_func,
                  void *next_func_arg,
                  pwdgrp_compare_func_t compare_func,
                  const void *target,
                  void **result)
{
  int found = 0;
  void *itr;
  init_func();
  while ((itr = next_func(next_func_arg)) != NULL) {
    if (compare_func(itr, target)) {
      found = 1;
      break;
    }
  }
  finit_func();
  if (found) {
    return 0;
  } else {
    *result = NULL;
    return -1;
  }
}

void *grp_next(void *arg)
{
  struct grp_next_arg_t *grp_arg = (struct grp_next_arg_t*)arg;
  getgrent_r(grp_arg->grp, grp_arg->buf,
             grp_arg->buflen, grp_arg->result);
  return (void*)(*grp_arg->result);
}

void *pwd_next(void *arg)
{
  struct pwd_next_arg_t *pwd_arg = (struct pwd_next_arg_t*)arg;
  getpwent_r(pwd_arg->pwd, pwd_arg->buf,
             pwd_arg->buflen, pwd_arg->result);
  return (void*)(*pwd_arg->result);
}
