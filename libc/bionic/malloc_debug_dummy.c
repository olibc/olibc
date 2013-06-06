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

#include "dlmalloc.h"
#include <sys/cdefs.h>
#include <stddef.h>
#include <malloc.h>

void* malloc(size_t bytes) {
    return dlmalloc(bytes);
}

void free(void* mem) {
    dlfree(mem);
}

void* calloc(size_t n_elements, size_t elem_size) {
    return dlcalloc(n_elements, elem_size);
}

void* realloc(void* oldMem, size_t bytes) {
    return dlrealloc(oldMem, bytes);
}

void* memalign(size_t alignment, size_t bytes) {
    return dlmemalign(alignment, bytes);
}

size_t malloc_usable_size(const void* mem) {
    return dlmalloc_usable_size(mem);
}

void* valloc(size_t bytes) {
    return dlvalloc(bytes);
}

void* pvalloc(size_t bytes) {
    return dlpvalloc(bytes);
}

int posix_memalign(void** memptr, size_t alignment, size_t size) {
    return dlposix_memalign(memptr, alignment, size);
}

struct mallinfo mallinfo() {
    return dlmallinfo();
}

#ifndef LIBC_STATIC
__LIBC_HIDDEN__ void malloc_debug_init() {
}

__LIBC_HIDDEN__ void malloc_debug_fini() {
}
#endif
