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

#include <pwd.h>
#include <stdio.h>
#include <errno.h>
#include <paths.h>
#include "passwd_private.h"

static FILE *pwf = NULL;

void setpwent() {
  if (pwf) {
    rewind(pwf);
  }
}

void endpwent() {
  if (pwf) {
    fclose(pwf);
    pwf = NULL;
  }
}

int getpwent_r(struct passwd *pwbuf, char *buf,
               size_t buflen, struct passwd **pwbufp)
{
  if (pwf == NULL) {
    pwf = fopen(_PATH_PASSWD, "re");
    if (pwf == NULL) {
      return errno;
    }
  }
  return fgetpwent_r(pwf, pwbuf, buf, buflen, pwbufp);
}
