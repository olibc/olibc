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
#include "passwd_private.h"

int putpwent(const struct passwd *p, FILE *fp)
{
  if (fp == NULL || p == NULL) {
    return -1;
  } else {
    fprintf(fp, "%s:%s:%d:%d:%s:%s:%s",
                p->pw_name, p->pw_passwd,
                p->pw_uid, p->pw_gid,
                p->pw_gecos, p->pw_dir,
                p->pw_shell);
    return 0;
  }
}
