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

#ifndef SCOPED_READDIR_H
#define SCOPED_READDIR_H

#include <dirent.h>
#include <stdbool.h>

typedef struct dirent dirent;
typedef struct ScopedReaddir ScopedReaddir;
struct ScopedReaddir {
  DIR* dir_;
};

static inline
void ScopedReaddir_init(ScopedReaddir* sr, const char* path) {
  sr->dir_ = opendir(path);
}

static inline
void ScopedReaddir_fini(ScopedReaddir* sr) {
  if (sr->dir_ != NULL) {
    closedir(sr->dir_);
  }
}

static inline
bool ScopedReaddir_IsBad(ScopedReaddir* sr) {
  return sr->dir_ == NULL;
}

static inline
dirent* ScopedReaddir_ReadEntry(ScopedReaddir* sr) {
  return readdir(sr->dir_);
}


#endif // SCOPED_READDIR_H
