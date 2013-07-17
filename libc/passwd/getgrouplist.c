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

int getgrouplist(const char *user, gid_t group,
                 gid_t *groups, int *ngroups)
{
  int max_group_size = *ngroups;
  int group_size = 1;
  struct group *grp;
  gid_t *group_list = groups;
  if (max_group_size < 1) {
    return -1;
  }
  *group_list++ = group;

  setgrent();
  while ((grp = getgrent())) {
    if (grp->gr_mem == NULL) {
      continue;
    }
    char **members = grp->gr_mem;
    while (*members) {
      if (strcmp(*members, user) == 0) {
        group_size++;
        if (group_size < max_group_size) {
          *group_list++ = grp->gr_gid;
        }
        break;
      }
    }
  }
  endgrent();
  *ngroups = group_size;
  if (group_size < max_group_size) {
    return group_size;
  } else {
    return -1;
  }
}
