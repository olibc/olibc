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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

char *get_current_dir_name()
{
  char *pwd = getenv("PWD");
  char buf[PATH_MAX];
  char *pwd_path;
  struct stat a, b;
  if (pwd && !stat(".", &a) && !stat(pwd, &b) &&
      (a.st_dev == b.st_dev) && (a.st_ino == b.st_ino)) {
    pwd_path = pwd;
  } else {
    if (getcwd(buf, sizeof(buf))) {
      pwd_path = buf;
    } else {
      return NULL;
    }
  }
  return strdup(pwd_path);
}
