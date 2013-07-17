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
#include <stdio.h>
#include <errno.h>
#include <paths.h>

static FILE *grf = NULL;

void setgrent(void)
{
  if (grf) {
    rewind(grf);
  }
}

void endgrent(void)
{
  if (grf) {
    fclose(grf);
    grf = NULL;
  }
}

int getgrent_r(struct group *gbuf, char *buf,
               size_t buflen, struct group **gbufp)
{
  if (grf == NULL) {
    grf = fopen(_PATH_GROUP, "re");
    if (grf == NULL) {
      return errno;
    }
  }
  return fgetgrent_r(grf, gbuf, buf, buflen, gbufp);
}
