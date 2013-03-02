/*
 * Copyright (C) 2013 The Android Open Source Project
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

#ifndef PTHREAD_ACCESSOR_H
#define PTHREAD_ACCESSOR_H

#include <stdbool.h>
#include <pthread.h>

#include "pthread_internal.h"

typedef struct pthread_accessor pthread_accessor;
struct pthread_accessor {
  pthread_internal_t* thread_;
  bool is_locked_;
};

static inline
void pthread_accessor_Unlock(pthread_accessor *pa) {
  if (pa->is_locked_) {
    pa->is_locked_ = false;
    pa->thread_ = NULL;
    pthread_mutex_unlock(&gThreadListLock);
  }
}

static inline
pthread_internal_t* pthread_accessor_get(const pthread_accessor *pa) {
  return pa->thread_;
}

static inline
void pthread_accessor_Lock(pthread_accessor *pa) {
  pthread_mutex_lock(&gThreadListLock);
  pa->is_locked_ = true;
}

static inline
void pthread_accessor_init(pthread_accessor *pa, pthread_t desired_thread) {
  pthread_accessor_Lock(pa);
  for (pa->thread_ = gThreadList; pa->thread_ != NULL; pa->thread_ = pa->thread_->next) {
    if (pa->thread_ == (pthread_internal_t*)(desired_thread)) {
      break;
    }
  }
}

static inline
void pthread_accessor_fini(pthread_accessor *pa) {
  pthread_accessor_Unlock(pa);
}

#endif // PTHREAD_ACCESSOR_H
