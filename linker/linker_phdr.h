/*
 * Copyright (C) 2012 The Android Open Source Project
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
#ifndef LINKER_PHDR_H
#define LINKER_PHDR_H

/* Declarations related to the ELF program header table and segments.
 *
 * The design goal is to provide an API that is as close as possible
 * to the ELF spec, and does not depend on linker-specific data
 * structures (e.g. the exact layout of struct soinfo).
 */

#include "linker.h"

typedef struct ElfReader ElfReader;
struct ElfReader {
  const char* name_;
  int fd_;

  Elf_Ehdr header_;
  size_t phdr_num_;

  void* phdr_mmap_;
  Elf_Phdr* phdr_table_;
  Elf_Addr phdr_size_;

  // First page of reserved address space.
  void* load_start_;
  // Size in bytes of reserved address space.
  Elf_Addr load_size_;
  // Load bias.
  Elf_Addr load_bias_;

  // Loaded phdr.
  const Elf_Phdr* loaded_phdr_;
};

void ElfReader_init(ElfReader* er, const char* name, int fd);
void ElfReader_fini(ElfReader* er);

bool ElfReader_Load(ElfReader* er);

static inline
size_t ElfReader_phdr_count(ElfReader* er) {
  return er->phdr_num_;
}

static inline
Elf_Addr ElfReader_load_start(ElfReader* er) {
  return (Elf_Addr)(er->load_start_);
}

static inline
Elf_Addr ElfReader_load_size(ElfReader* er) {
  return er->load_size_;
}

static inline
Elf_Addr ElfReader_load_bias(ElfReader* er) {
  return er->load_bias_;
}

static inline
const Elf_Phdr* ElfReader_loaded_phdr(ElfReader* er) {
  return er->loaded_phdr_;
}

bool ElfReader_ReadElfHeader(ElfReader* er);
bool ElfReader_VerifyElfHeader(ElfReader* er);
bool ElfReader_ReadProgramHeader(ElfReader* er);
bool ElfReader_ReserveAddressSpace(ElfReader* er);
bool ElfReader_LoadSegments(ElfReader* er);
bool ElfReader_FindPhdr(ElfReader* er);
bool ElfReader_CheckPhdr(ElfReader* er, Elf_Addr);

size_t phdr_table_get_load_size(const Elf_Phdr* phdr_table, size_t phdr_count,
                                Elf_Addr* min_vaddr, Elf_Addr* max_vaddr);

int phdr_table_protect_segments(const Elf_Phdr* phdr_table, size_t phdr_count, Elf_Addr load_bias);

int phdr_table_unprotect_segments(const Elf_Phdr* phdr_table, size_t phdr_count, Elf_Addr load_bias);

int phdr_table_protect_gnu_relro(const Elf_Phdr* phdr_table, size_t phdr_count, Elf_Addr load_bias);


#ifdef ANDROID_ARM_LINKER
int phdr_table_get_arm_exidx(const Elf_Phdr* phdr_table, size_t phdr_count, Elf_Addr load_bias,
                             Elf_Addr** arm_exidx, unsigned* arm_exidix_count);
#endif

void phdr_table_get_dynamic_section(const Elf_Phdr* phdr_table, size_t phdr_count,
                                    Elf_Addr load_bias,
                                    Elf_Dyn** dynamic, size_t* dynamic_count, Elf_Word* dynamic_flags);

#endif /* LINKER_PHDR_H */
