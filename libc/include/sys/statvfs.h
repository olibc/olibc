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
#ifndef _SYS_STATVFS_H_
#define _SYS_STATVFS_H_

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

enum {
  ST_RDONLY = 1,
#define ST_RDONLY ST_RDONLY
  ST_NOSUID = 2,
#define ST_NOSUID ST_NOSUID
  ST_NODEV = 4,
#define ST_NODEV ST_NODEV
  ST_NOEXEC = 8,
#define ST_NOEXEC ST_NOEXEC
  ST_SYNCHRONOUS = 16,
#define ST_SYNCHRONOUS ST_SYNCHRONOUS
  ST_MANDLOCK = 64,
#define ST_MANDLOCK ST_MANDLOCK
  ST_WRITE = 128,
#define ST_WRITE ST_WRITE
  ST_APPEND = 256,
#define ST_APPEND ST_APPEND
  ST_IMMUTABLE = 512,
#define ST_IMMUTABLE ST_IMMUTABLE
  ST_NOATIME = 1024,
#define ST_NOATIME ST_NOATIME
  ST_NODIRATIME = 2048,
#define ST_NODIRATIME ST_NODIRATIME
  ST_RELATIME = 4096
#define ST_RELATIME ST_RELATIME
};

struct statvfs {
  uint32_t f_bsize;
  uint32_t f_frsize;
  uint64_t f_blocks;
  uint64_t f_bfree;
  uint64_t f_bavail;
  uint64_t f_files;
  uint64_t f_ffree;
  uint64_t f_favail;
  __kernel_fsid_t f_fsid;
  uint32_t f_flag;
  uint32_t f_namemax;
};

extern int statvfs (const char *__restrict,
                    struct statvfs *__restrict);
extern int fstatvfs (int, struct statvfs *);


__END_DECLS

#endif /* _SYS_STATVFS_H_ */
