#
# Copyright (C) 2012 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

# -----------------------------------------------------------------------------
# Benchmarks.
# -----------------------------------------------------------------------------

benchmark_c_flags = \
    -O2 \
    -Wall -Wextra \
    -Werror \
    -fno-builtin \

benchmark_src_files = \
    benchmark_main.cpp \
    math_benchmark.cpp \
    string_benchmark.cpp \
    time_benchmark.cpp \

# Build benchmarks for the device (with bionic's .so). Run with:
#   adb shell bionic-benchmarks
ifneq ($(EXTRA_STLPORT_SUPPORT),)
include $(CLEAR_VARS)
LOCAL_MODULE := bionic-benchmarks
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
LOCAL_CFLAGS += $(benchmark_c_flags)
LOCAL_C_INCLUDES += external/stlport/stlport bionic/ bionic/libstdc++/include
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_STATIC_LIBRARIES += libstlport_static libstdc++ libm libc
LOCAL_SRC_FILES := $(benchmark_src_files)
include $(BUILD_EXECUTABLE)
endif

# -----------------------------------------------------------------------------
# Unit tests.
# -----------------------------------------------------------------------------

test_c_flags = \
    -fstack-protector-all \
    -g \
    -Wall -Wextra \
    -Werror \
    -fno-builtin \

test_src_files = \
    dirent_test.cpp \
    eventfd_test.cpp \
    fenv_test.cpp \
    fortify1_test.cpp \
    fortify2_test.cpp \
    getauxval_test.cpp \
    getcwd_test.cpp \
    libc_logging_test.cpp \
    libgen_test.cpp \
    malloc_test.cpp \
    math_test.cpp \
    netdb_test.cpp \
    pthread_test.cpp \
    regex_test.cpp \
    signal_test.cpp \
    stack_protector_test.cpp \
    stdio_test.cpp \
    stdlib_test.cpp \
    string_test.cpp \
    strings_test.cpp \
    stubs_test.cpp \
    time_test.cpp \
    unistd_test.cpp \

ifeq ($(STPCPY_IMPL),true)
  test_c_flags += -DSTPCPY_IMPL
endif

ifeq ($(STRCHRNUL_IMPL),true)
  test_c_flags += -DSTRCHRNUL_IMPL
endif

ifeq ($(MEMPCPY_IMPL),true)
  test_c_flags += -DMEMPCPY_IMPL
endif

ifeq ($(SYSVIPC_SUPPORT),true)
  test_src_files += sysvipc_test.cpp
endif

test_dynamic_ldflags = -Wl,--export-dynamic -Wl,-u,DlSymTestFunction
test_dynamic_src_files = \
    dlfcn_test.cpp \

# Build tests for the device (with bionic's .so). Run with:
#   adb shell /data/nativetest/bionic-unit-tests/bionic-unit-tests
ifneq ($(EXTRA_STLPORT_SUPPORT),)
include $(CLEAR_VARS)
LOCAL_MODULE := bionic-unit-tests
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
LOCAL_CFLAGS += $(test_c_flags)
LOCAL_LDFLAGS += $(test_dynamic_ldflags)
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SRC_FILES := $(test_src_files) $(test_dynamic_src_files)
include $(BUILD_NATIVE_TEST)
endif

# Build tests for the device (with bionic's .a). Run with:
#   adb shell /data/nativetest/bionic-unit-tests-static/bionic-unit-tests-static
ifneq ($(EXTRA_STLPORT_SUPPORT),)
include $(CLEAR_VARS)
LOCAL_MODULE := bionic-unit-tests-static
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
LOCAL_CFLAGS += $(test_c_flags)
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_SRC_FILES := $(test_src_files)
LOCAL_STATIC_LIBRARIES += libstlport_static libstdc++ libm libc
include $(BUILD_NATIVE_TEST)
endif

# -----------------------------------------------------------------------------
# Test library for the unit tests.
# -----------------------------------------------------------------------------

# Build no-elf-hash-table-library.so to test dlopen(3) on a library that
# only has a GNU-style hash table. MIPS doesn't support GNU hash style.
ifneq ($(TARGET_ARCH),mips)
include $(CLEAR_VARS)
LOCAL_MODULE := no-elf-hash-table-library
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
LOCAL_SRC_FILES := empty.cpp
LOCAL_LDFLAGS := -Wl,--hash-style=gnu
include $(BUILD_SHARED_LIBRARY)
endif

# -----------------------------------------------------------------------------
# Unit tests built against glibc.
# -----------------------------------------------------------------------------

# Build tests for the host (with glibc).
# Note that this will build against glibc, so it's not useful for testing
# bionic's implementation, but it does let you use glibc as a reference
# implementation for testing the tests themselves.
ifeq ($(HOST_OS)-$(HOST_ARCH),linux-x86)
include $(CLEAR_VARS)
LOCAL_MODULE := bionic-unit-tests-glibc
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
LOCAL_CFLAGS += $(test_c_flags)
LOCAL_LDFLAGS += -lpthread -ldl
LOCAL_LDFLAGS += $(test_dynamic_ldflags)
LOCAL_SRC_FILES := $(test_src_files) $(test_dynamic_src_files)
include $(BUILD_HOST_NATIVE_TEST)
endif
