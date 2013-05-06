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
#ifndef _SYS_SWAP_H_
#define _SYS_SWAP_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

enum {
  SWAP_FLAG_PREFER     = 0x8000,
  SWAP_FLAG_PRIO_MASK  = 0x7fff,
  SWAP_FLAG_PRIO_SHIFT = 0x0000,
  SWAP_FLAG_DISCARD    = 0x10000
};

int swapon(const char *path, int swapflags);
int swapoff(const char *path);

__END_DECLS

#endif /* _SYS_SWAP_H_ */
