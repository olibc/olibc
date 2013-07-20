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

static int pwd_find_by_uid(void *pwd,
                           const void *pwd_target)
{
  struct passwd *_pwd = (struct passwd *)(pwd);
  uid_t _pwd_target = *((const pid_t *)pwd_target);
  return _pwd->pw_uid == _pwd_target;
}

int getpwuid_r(uid_t uid, struct passwd *pwd,
               char *buf, size_t buflen, struct passwd **result)
{
  uid_t pwd_target = uid;
  return __pwdgrp_find((const void*)&pwd_target, (void*)pwd,
                       buf, buflen, (void**)result,
                       setpwent, endpwent,
                       pwd_next,
                       pwd_find_by_uid);
}
