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

#include "statvfs_internal.h"
#include <paths.h>
#include <mntent.h>

void statvfs_internal(struct statvfs *buf,
                      struct statfs *stfs) {
  buf->f_bsize = stfs->f_bsize;
  buf->f_frsize = stfs->f_bsize;
  buf->f_blocks = stfs->f_blocks;
  buf->f_bfree = stfs->f_bfree;
  buf->f_bavail = stfs->f_bavail;
  buf->f_files = stfs->f_files;
  buf->f_ffree = stfs->f_ffree;
  buf->f_fsid = stfs->f_fsid;
  buf->f_namemax = stfs->f_namelen;
  /* f_favail is just follow glibc/uClib. */
  buf->f_favail = stfs->f_ffree;

  buf->f_flag = 0;
}
