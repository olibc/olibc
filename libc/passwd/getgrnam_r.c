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
#include <string.h>

#include "passwd_private.h"

static int grp_find_by_name(void *grp,
                            const void *grp_target)
{
  struct group *_grp = (struct group *)(grp);
  const char *_grp_target = (const char *)grp_target;
  return (strcmp(_grp->gr_name, _grp_target) == 0);
}

int getgrnam_r(const char *name, struct group *grp,
               char *buf, size_t buflen, struct group **result)
{
  const char *grp_target = name;
  struct grp_next_arg_t grp_next_arg;
  grp_next_arg.grp = grp;
  grp_next_arg.buf = buf;
  grp_next_arg.buflen = buflen;
  grp_next_arg.result = result;

  return __pwdgrp_find(setgrent, endgrent,
                       grp_next, &grp_next_arg,
                       grp_find_by_name, grp_target,
                       (void**)result);
}
