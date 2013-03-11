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

#include <dirent.h>

#include <errno.h>
#include <stdlib.h>

#include "private/ScopedReaddir.h"

// A smart pointer to the scandir dirent**.
typedef struct ScandirResult ScandirResult;
struct ScandirResult {
  dirent** names_;
  size_t size_;
  size_t capacity_;
};

static dirent* CopyDirent(dirent* original);

static void ScandirResult_init(ScandirResult* sr) {
   sr->names_ = NULL;
   sr->size_ = 0;
   sr->capacity_ = 0;
}

static void ScandirResult_fini(ScandirResult* sr) {
  while (sr->size_ > 0) {
    free(sr->names_[--sr->size_]);
  }
  free(sr->names_);
}

static size_t ScandirResult_size(ScandirResult* sr) {
  return sr->size_;
}

static dirent** ScandirResult_release(ScandirResult* sr) {
  dirent** result = sr->names_;
  sr->names_ = NULL;
  sr->size_ = sr->capacity_ = 0;
  return result;
}

static bool ScandirResult_Add(ScandirResult* sr, dirent* entry) {
  if (sr->size_ >= sr->capacity_) {
    size_t new_capacity = sr->capacity_ + 32;
    dirent** new_names = (dirent**) realloc(sr->names_, new_capacity * sizeof(dirent*));
    if (new_names == NULL) {
      return false;
    }
    sr->names_ = new_names;
    sr->capacity_ = new_capacity;
  }

  dirent* copy = CopyDirent(entry);
  if (copy == NULL) {
    return false;
  }
  sr->names_[sr->size_++] = copy;
  return true;
}

static void ScandirResult_Sort(ScandirResult* sr, int (*comparator)(const dirent**, const dirent**)) {
  // If we have entries and a comparator, sort them.
  if (sr->size_ > 0 && comparator != NULL) {
    qsort(sr->names_, sr->size_, sizeof(dirent*), (int (*)(const void*, const void*)) comparator);
  }
}

static dirent* CopyDirent(dirent* original) {
  // Allocate the minimum number of bytes necessary, rounded up to a 4-byte boundary.
  size_t size = ((original->d_reclen + 3) & ~3);
  dirent* copy = (dirent*) malloc(size);
  memcpy(copy, original, original->d_reclen);
  return copy;
}

int scandir(const char* dirname, dirent*** name_list,
            int (*filter)(const dirent*),
            int (*comparator)(const dirent**, const dirent**)) {
  ScopedReaddir reader;
  ScopedReaddir_init(&reader, dirname);
  if (ScopedReaddir_IsBad(&reader)) {
    return -1;
  }

  ScandirResult names;
  ScandirResult_init(&names);
  dirent* entry;
  while ((entry = ScopedReaddir_ReadEntry(&reader)) != NULL) {
    // If we have a filter, skip names that don't match.
    if (filter != NULL && !(*filter)(entry)) {
      continue;
    }
    ScandirResult_Add(&names, entry);
  }

  ScandirResult_Sort(&names, comparator);

  size_t size = ScandirResult_size(&names);
  *name_list = ScandirResult_release(&names);
  ScandirResult_fini(&names);
  ScopedReaddir_fini(&reader);
  return size;
}
