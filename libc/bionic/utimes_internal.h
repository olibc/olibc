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

#ifndef _UTIMES_INTERNAL_H_
#define _UTIMES_INTERNAL_H_

#include <sys/time.h>
#include <sys/cdefs.h>

__LIBC_HIDDEN__ int utimes_internal(int fd,
                                    const char *path,
                                    int flags,
                                    const struct timeval tv[2]);

#endif /* _UTIMES_INTERNAL_H_ */
