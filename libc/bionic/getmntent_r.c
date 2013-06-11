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

struct mntent *getmntent_r(FILE *fp, struct mntent *mntbuf,
                           char *buf, int buflen) {
  if (!fp || !mntbuf || !buf) return NULL;
  while (fgets(buf, buflen, fp) != NULL) {
    int count;
    int pos[8];
    count = sscanf(buf, " %n%*s%n %n%*s%n %n%*s%n %n%*s%n %d %d",
                        &pos[0], &pos[1], &pos[2], &pos[3],
                        &pos[4], &pos[5], &pos[6], &pos[7],
                        &mntbuf->mnt_freq,
                        &mntbuf->mnt_passno);
    /* At least fs_spec and fs_file field is required! */
    if (count < 2) continue;
    /* This line is comment. */
    if (buf[pos[0]] == '#') continue;
    mntbuf->mnt_fsname = buf+pos[0];
    mntbuf->mnt_fsname[pos[1]] = '\0';
    mntbuf->mnt_dir = buf+pos[2];
    mntbuf->mnt_dir[pos[3]] = '\0';
    mntbuf->mnt_type = buf+pos[4];
    mntbuf->mnt_type[pos[5]] = '\0';
    mntbuf->mnt_opts = buf+pos[6];
    mntbuf->mnt_opts[pos[7]] = '\0';
  }
  return NULL;
}
