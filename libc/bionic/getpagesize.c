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
#include <sys/sysconf.h>
#include <limits.h>

// Portable code should use sysconf(_SC_PAGESIZE) directly instead.
int getpagesize() {
  return sysconf(_SC_PAGESIZE);
}

// These were accidentally declared in <unistd.h> because we stupidly used to inline
// getpagesize() and __getpageshift(). Needed for backwards compatibility with old NDK apps.
unsigned int __page_size = PAGE_SIZE;
unsigned int __page_shift = PAGE_SHIFT;
