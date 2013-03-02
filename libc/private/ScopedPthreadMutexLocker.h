/*
 * Copyright (C) 2012 The Android Open Source Project
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

#ifndef SCOPED_PTHREAD_MUTEX_LOCKER_H
#define SCOPED_PTHREAD_MUTEX_LOCKER_H

#include <pthread.h>

typedef struct ScopedPthreadMutexLocker ScopedPthreadMutexLocker;

struct ScopedPthreadMutexLocker {
  pthread_mutex_t* mu_;
};

static inline
void ScopedPthreadMutexLocker_init(ScopedPthreadMutexLocker *smu, pthread_mutex_t* mu) {
  smu->mu_ = mu;
  pthread_mutex_lock(smu->mu_);
}

static inline
void ScopedPthreadMutexLocker_fini(ScopedPthreadMutexLocker *smu) {
  pthread_mutex_unlock(smu->mu_);
}

#endif // SCOPED_PTHREAD_MUTEX_LOCKER_H
