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
#include <stddef.h>
#include <stdio.h>
#include "passwd_private.h"

int putgrent(const struct group *grp, FILE *fp)
{
  int rv = -1;
  if (grp != NULL && fp != NULL) {
    fprintf(fp, "%s:%s:%d:", grp->gr_name, grp->gr_passwd, grp->gr_gid);
    if (grp->gr_mem) {
      char **member = grp->gr_mem;
      const char *sep = "";
      while (*member) {
        fprintf(fp, "%s%s", sep, *member++);
        sep = ",";
      }
    }
  }
  return rv;
}
