/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <pthread.h>

#include <fcntl.h>
#include <stdio.h> // For snprintf.
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "pthread_accessor.h"
#include "pthread_internal.h"
#include "private/ErrnoRestorer.h"

// This value is not exported by kernel headers.
#define MAX_TASK_COMM_LEN 16
#define TASK_COMM_FMT "/proc/self/task/%d/comm"

int pthread_setname_np(pthread_t t, const char* thread_name) {
  ErrnoRestorer errno_restorer;
  ErrnoRestorer_init(&errno_restorer);

  if (thread_name == NULL) {
    ErrnoRestorer_fini(&errno_restorer);
    return EINVAL;
  }

  size_t thread_name_len = strlen(thread_name);
  if (thread_name_len >= MAX_TASK_COMM_LEN) {
    ErrnoRestorer_fini(&errno_restorer);
    return ERANGE;
  }

  // Changing our own name is an easy special case.
  if (t == pthread_self()) {
    int ret = prctl(PR_SET_NAME, thread_name) ? errno : 0;
    ErrnoRestorer_fini(&errno_restorer);
    return ret;
  }

  // We have to change another thread's name.
  pid_t tid = 0;
  {
    pthread_accessor thread;
    pthread_accessor_init(&thread, t);
    if (pthread_accessor_get(&thread) == NULL) {
      pthread_accessor_fini(&thread);
      ErrnoRestorer_fini(&errno_restorer);
      return ESRCH;
    }
    tid = pthread_accessor_get(&thread)->tid;
    pthread_accessor_fini(&thread);
  }  char comm_name[sizeof(TASK_COMM_FMT) + 8];
  snprintf(comm_name, sizeof(comm_name), TASK_COMM_FMT, tid);
  int fd = open(comm_name, O_WRONLY);
  if (fd == -1) {
    int ret = errno;
    ErrnoRestorer_fini(&errno_restorer);
    return ret;
  }
  ssize_t n = TEMP_FAILURE_RETRY(write(fd, thread_name, thread_name_len));
  close(fd);

  if (n < 0) {
    int ret = errno;
    ErrnoRestorer_fini(&errno_restorer);
    return ret;
  } else if (n != (ssize_t)(thread_name_len)) {
    ErrnoRestorer_fini(&errno_restorer);
    return EIO;
  }
  ErrnoRestorer_fini(&errno_restorer);
  return 0;
}
