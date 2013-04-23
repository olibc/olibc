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

#include <stdarg.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "ipc_private.h"

int semctl(int semid, int semnum, int cmd, ...) {
  union semun arg;
  va_list ap;

  va_start (ap, cmd);
  arg = va_arg (ap, union semun);
  va_end (ap);
  return ipc(SEMCTL, semid, semnum, cmd, &arg, 0);
}
