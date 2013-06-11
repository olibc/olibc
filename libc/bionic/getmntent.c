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

#include <mntent.h>
#include <pthread.h>

#define GETMNTENT_BUFFER_SIZE 4096

static pthread_mutex_t gGetmntentMutex = PTHREAD_MUTEX_INITIALIZER;

struct mntent *getmntent(FILE *fp) {
  static struct mntent mnt;
  static char *buf = NULL;
  struct mntent *ret;
  pthread_mutex_lock(&gGetmntentMutex);
  if (buf == NULL) {
    buf = malloc(GETMNTENT_BUFFER_SIZE);
    if (buf == NULL) {
      return NULL;
    }
  }
  ret = getmntent_r(fp, &mnt, buf, GETMNTENT_BUFFER_SIZE);
  pthread_mutex_unlock(&gGetmntentMutex);
  return ret;
}
