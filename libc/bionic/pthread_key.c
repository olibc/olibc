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

#include "bionic_tls.h"
#include "pthread_internal.h"

/* A technical note regarding our thread-local-storage (TLS) implementation:
 *
 * There can be up to BIONIC_TLS_SLOTS independent TLS keys in a given process,
 * The keys below TLS_SLOT_FIRST_USER_SLOT are reserved for Bionic to hold
 * special thread-specific variables like errno or a pointer to
 * the current thread's descriptor. These entries cannot be accessed through
 * pthread_getspecific() / pthread_setspecific() or pthread_key_delete()
 *
 * The 'tls_map_t' type defined below implements a shared global map of
 * currently created/allocated TLS keys and the destructors associated
 * with them.
 *
 * The global TLS map simply contains a bitmap of allocated keys, and
 * an array of destructors.
 *
 * Each thread has a TLS area that is a simple array of BIONIC_TLS_SLOTS void*
 * pointers. the TLS area of the main thread is stack-allocated in
 * __libc_init_common, while the TLS area of other threads is placed at
 * the top of their stack in pthread_create.
 *
 * When pthread_key_delete() is called it will erase the key's bitmap bit
 * and its destructor, and will also clear the key data in the TLS area of
 * all created threads. As mandated by Posix, it is the responsibility of
 * the caller of pthread_key_delete() to properly reclaim the objects that
 * were pointed to by these data fields (either before or after the call).
 */

#define TLSMAP_BITS       32
#define TLSMAP_WORDS      ((BIONIC_TLS_SLOTS+TLSMAP_BITS-1)/TLSMAP_BITS)
#define TLSMAP_WORD(m,k)  (m).map[(k)/TLSMAP_BITS]
#define TLSMAP_MASK(k)    (1U << ((k)&(TLSMAP_BITS-1)))

static inline bool IsValidUserKey(pthread_key_t key) {
  return (key >= TLS_SLOT_FIRST_USER_SLOT && key < BIONIC_TLS_SLOTS);
}

typedef void (*key_destructor_t)(void*);

typedef struct tls_map_t tls_map_t;

struct tls_map_t {
  bool is_initialized;

  /* bitmap of allocated keys */
  uint32_t map[TLSMAP_WORDS];

  key_destructor_t key_destructors[BIONIC_TLS_SLOTS];
};

typedef struct ScopedTlsMapAccess ScopedTlsMapAccess;

struct ScopedTlsMapAccess {
};

static tls_map_t ScopedTlsMapAccess_s_tls_map_;
static pthread_mutex_t ScopedTlsMapAccess_s_tls_map_lock_;

static inline
void ScopedTlsMapAccess_Lock(ScopedTlsMapAccess *tls_map __unused) {
  pthread_mutex_lock(&ScopedTlsMapAccess_s_tls_map_lock_);
}

static inline
void ScopedTlsMapAccess_Unlock(ScopedTlsMapAccess *tls_map __unused) {
  pthread_mutex_unlock(&ScopedTlsMapAccess_s_tls_map_lock_);
}

static inline
bool ScopedTlsMapAccess_IsInUse(ScopedTlsMapAccess *tls_map __unused, pthread_key_t key) {
  return (TLSMAP_WORD(ScopedTlsMapAccess_s_tls_map_, key) & TLSMAP_MASK(key)) != 0;
}

static inline
void ScopedTlsMapAccess_SetInUse(ScopedTlsMapAccess *tls_map __unused, pthread_key_t key, void (*key_destructor)(void*)) {
  TLSMAP_WORD(ScopedTlsMapAccess_s_tls_map_, key) |= TLSMAP_MASK(key);
  ScopedTlsMapAccess_s_tls_map_.key_destructors[key] = key_destructor;
}

static inline
void ScopedTlsMapAccess_init(ScopedTlsMapAccess *tls_map __unused) {
  ScopedTlsMapAccess_Lock(tls_map);

  // If this is the first time the TLS map has been accessed,
  // mark the slots belonging to well-known keys as being in use.
  // This isn't currently necessary because the well-known keys
  // can only be accessed directly by bionic itself, do not have
  // destructors, and all the functions that touch the TLS map
  // start after the maximum well-known slot.
  if (!ScopedTlsMapAccess_s_tls_map_.is_initialized) {
    pthread_key_t key;
    for (key = 0; key < TLS_SLOT_FIRST_USER_SLOT; ++key) {
      ScopedTlsMapAccess_SetInUse(tls_map, key, NULL);
    }
    ScopedTlsMapAccess_s_tls_map_.is_initialized = true;
  }
}

static inline
void ScopedTlsMapAccess_fini(ScopedTlsMapAccess *tls_map __unused) {
  ScopedTlsMapAccess_Unlock(tls_map);
}

static inline
int ScopedTlsMapAccess_CreateKey(ScopedTlsMapAccess *tls_map __unused, pthread_key_t* result, void (*key_destructor)(void*)) {
  // Take the first unallocated key.
  int key;
  for (key = 0; key < BIONIC_TLS_SLOTS; ++key) {
    if (!ScopedTlsMapAccess_IsInUse(tls_map, key)) {
      ScopedTlsMapAccess_SetInUse(tls_map, key, key_destructor);
      *result = key;
      return 0;
    }
  }

  // We hit PTHREAD_KEYS_MAX. POSIX says EAGAIN for this case.
  return EAGAIN;
}

static inline
void ScopedTlsMapAccess_DeleteKey(ScopedTlsMapAccess *tls_map __unused, pthread_key_t key) {
  TLSMAP_WORD(ScopedTlsMapAccess_s_tls_map_, key) &= ~TLSMAP_MASK(key);
  ScopedTlsMapAccess_s_tls_map_.key_destructors[key] = NULL;
}

// Called from pthread_exit() to remove all TLS key data
// from this thread's TLS area. This must call the destructor of all keys
// that have a non-NULL data value and a non-NULL destructor.
static inline
void ScopedTlsMapAccess_CleanAll(ScopedTlsMapAccess *tls_map __unused) {
  void** tls = (void**)__get_tls();

  // Because destructors can do funky things like deleting/creating other
  // keys, we need to implement this in a loop.
  int rounds;
  for (rounds = PTHREAD_DESTRUCTOR_ITERATIONS; rounds > 0; --rounds) {
    size_t called_destructor_count = 0;
    int key;
    for (key = 0; key < BIONIC_TLS_SLOTS; ++key) {
      if (ScopedTlsMapAccess_IsInUse(tls_map, key)) {
        void* data = tls[key];
        void (*key_destructor)(void*) = ScopedTlsMapAccess_s_tls_map_.key_destructors[key];

        if (data != NULL && key_destructor != NULL) {
          // we need to clear the key data now, this will prevent the
          // destructor (or a later one) from seeing the old value if
          // it calls pthread_getspecific() for some odd reason

          // we do not do this if 'key_destructor == NULL' just in case another
          // destructor function might be responsible for manually
          // releasing the corresponding data.
          tls[key] = NULL;

          // because the destructor is free to call pthread_key_create
          // and/or pthread_key_delete, we need to temporarily unlock
          // the TLS map
          ScopedTlsMapAccess_Unlock(tls_map);
          (*key_destructor)(data);
          ScopedTlsMapAccess_Lock(tls_map);
          ++called_destructor_count;
        }
      }
    }

    // If we didn't call any destructors, there is no need to check the TLS data again.
    if (called_destructor_count == 0) {
      break;
    }
  }
}

__LIBC_HIDDEN__ void pthread_key_clean_all() {
  ScopedTlsMapAccess tls_map;
  ScopedTlsMapAccess_init(&tls_map);
  ScopedTlsMapAccess_CleanAll(&tls_map);
  ScopedTlsMapAccess_fini(&tls_map);
}

int pthread_key_create(pthread_key_t* key, void (*key_destructor)(void*)) {
  ScopedTlsMapAccess tls_map;
  ScopedTlsMapAccess_init(&tls_map);
  int ret = ScopedTlsMapAccess_CreateKey(&tls_map, key, key_destructor);
  ScopedTlsMapAccess_fini(&tls_map);
  return ret;
}

// Deletes a pthread_key_t. note that the standard mandates that this does
// not call the destructors for non-NULL key values. Instead, it is the
// responsibility of the caller to properly dispose of the corresponding data
// and resources, using any means it finds suitable.
int pthread_key_delete(pthread_key_t key) {
  ScopedTlsMapAccess tls_map;
  ScopedTlsMapAccess_init(&tls_map);

  if (!IsValidUserKey(key) || !ScopedTlsMapAccess_IsInUse(&tls_map, key)) {
    ScopedTlsMapAccess_fini(&tls_map);
    return EINVAL;
  }

  // Clear value in all threads.
  pthread_mutex_lock(&gThreadListLock);
  for (pthread_internal_t*  t = gThreadList; t != NULL; t = t->next) {
    // Skip zombie threads. They don't have a valid TLS area any more.
    // Similarly, it is possible to have t->tls == NULL for threads that
    // were just recently created through pthread_create() but whose
    // startup trampoline (__thread_entry) hasn't been run yet by the
    // scheduler. t->tls will also be NULL after a thread's stack has been
    // unmapped but before the ongoing pthread_join() is finished.
    if ((t->attr.flags & PTHREAD_ATTR_FLAG_ZOMBIE) || t->tls == NULL) {
      continue;
    }

    t->tls[key] = NULL;
  }
  ScopedTlsMapAccess_DeleteKey(&tls_map, key);

  pthread_mutex_unlock(&gThreadListLock);
  ScopedTlsMapAccess_fini(&tls_map);
  return 0;
}

void* pthread_getspecific(pthread_key_t key) {
  if (!IsValidUserKey(key)) {
    return NULL;
  }

  // For performance reasons, we do not lock/unlock the global TLS map
  // to check that the key is properly allocated. If the key was not
  // allocated, the value read from the TLS should always be NULL
  // due to pthread_key_delete() clearing the values for all threads.
  return (void *)(((unsigned *)__get_tls())[key]);
}

int pthread_setspecific(pthread_key_t key, const void* ptr) {
  ScopedTlsMapAccess tls_map;
  ScopedTlsMapAccess_init(&tls_map);

  if (!IsValidUserKey(key) || !ScopedTlsMapAccess_IsInUse(&tls_map, key)) {
    ScopedTlsMapAccess_fini(&tls_map);
    return EINVAL;
  }

  ((uint32_t *)__get_tls())[key] = (uint32_t)ptr;
  ScopedTlsMapAccess_fini(&tls_map);
  return 0;
}
