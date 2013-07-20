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
#include "passwd_private.h"

static int grp_find_by_gid(void *grp,
                           const void *grp_target)
{
  struct group *_grp = (struct group *)(grp);
  gid_t _grp_target= *((const gid_t *)grp_target);
  return _grp->gr_gid == _grp_target;
}

int getgrgid_r(gid_t gid, struct group *grp,
               char *buf, size_t buflen, struct group **result)
{
  gid_t grp_target = gid;
  return __pwdgrp_find((const void*)&grp_target, (void*)grp,
                       buf, buflen, (void**)result,
                       setgrent, endgrent,
                       grp_next, grp_find_by_gid);
}
