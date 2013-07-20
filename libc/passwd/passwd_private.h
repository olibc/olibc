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

#ifndef _PASSWD_PRIVATE_H_
#define _PASSWD_PRIVATE_H_

#include <shadow.h>
#include <pwd.h>
#include <grp.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#define SPWD_BUFFER_SIZE 4096
#define PWD_BUFFER_SIZE 4096
#define GRP_BUFFER_SIZE 4096

extern __LIBC_HIDDEN__ char __spwd_buf[SPWD_BUFFER_SIZE];
extern __LIBC_HIDDEN__ struct spwd __spwd_spwdbuf;

extern __LIBC_HIDDEN__ char __pwd_buf[PWD_BUFFER_SIZE];
extern __LIBC_HIDDEN__ struct passwd __pwd_pwdbuf;

extern __LIBC_HIDDEN__ char grp_buf[GRP_BUFFER_SIZE];
extern __LIBC_HIDDEN__ struct group grp_gbuf;

typedef int (*pwdgrp_compare_func_t)(void *pwdgrp,
                                     const void *target);

typedef int (*pwdgrp_next_func_t)(void *pwdgrpbuf,
                                  char *buf,
                                  size_t buflen,
                                  void **pwdgrpbufp);

typedef void (*pwdgrp_init_func_t)();
typedef void (*pwdgrp_finit_func_t)();

struct grp_next_arg_t {
  struct group *grp;
  char *buf;
  size_t buflen;
  struct group **result;
};

struct pwd_next_arg_t {
  struct passwd *pwd;
  char *buf;
  size_t buflen;
  struct passwd **result;
};

__LIBC_HIDDEN__ int grp_next(void *pwdgrpbuf,
                             char *buf,
                             size_t buflen,
                             void **pwdgrpbufp);
__LIBC_HIDDEN__ int pwd_next(void *pwdgrpbuf,
                             char *buf,
                             size_t buflen,
                             void **pwdgrpbufp);

__LIBC_HIDDEN__ int __pwdgrp_find(const void *target,
                                  void *pwdgrpbuf,
                                  char *buf,
                                  size_t buflen,
                                  void *pwdgrpbufp,
                                  pwdgrp_init_func_t init_func,
                                  pwdgrp_finit_func_t finit_func,
                                  pwdgrp_next_func_t next_func,
                                  pwdgrp_compare_func_t compare_func);

#endif /* _PASSWD_PRIVATE_H_ */
