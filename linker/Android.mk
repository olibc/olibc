LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(TARGET_ARCH),x86)
    linker_begin_extension := c
else
    linker_begin_extension := S
endif

LOCAL_SRC_FILES:= \
    arch/$(TARGET_ARCH)/begin.$(linker_begin_extension) \
    dlfcn.c \
    linker.c \
    linker_environ.c \
    linker_phdr.c \
    rt.c \

LOCAL_LDFLAGS := -shared -Wl,--exclude-libs,ALL

LOCAL_CFLAGS += -fno-stack-protector \
        -Wstrict-overflow=5 \
        -fvisibility=hidden \
        -Wall -Wextra -Werror \

ifeq ($(DEBUGGERD_SYSTEM_SUPPORT),true)
    LOCAL_SRC_FILES+= \
        debugger.c
endif

# We need to access Bionic private headers in the linker.
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../libc/

ifeq ($(TARGET_ARCH),arm)
    LOCAL_CFLAGS += -DANDROID_ARM_LINKER
endif

ifeq ($(TARGET_ARCH),mips)
    LOCAL_CFLAGS += -DANDROID_MIPS_LINKER
endif

ifeq ($(TARGET_ARCH),x86)
    LOCAL_CFLAGS += -DANDROID_X86_LINKER
endif

ifeq ($(TARGET_ARCH),x86_64)
    LOCAL_CFLAGS += -DANDROID_X86_64_LINKER
endif

# Use the AT_SECURE auxv flag to determine whether to enable secure mode
ifeq ($(USE_AT_SECURE),true)
    LOCAL_CFLAGS += -DUSE_AT_SECURE
endif

# debug traces
LOCAL_CFLAGS += -DTRACE_DEBUG=$(if $(filter true,$(LINKER_TRACE_DEBUG)),1,0)
LOCAL_CFLAGS += -DDO_TRACE_LOOKUP=$(if $(filter true,$(LINKER_TRACE_LOOKUP)),1,0)
LOCAL_CFLAGS += -DDO_TRACE_RELO=$(if $(filter true,$(LINKER_TRACE_RELO)),1,0)
LOCAL_CFLAGS += -DTIMING=$(if $(filter true,$(LINKER_TIMING)),1,0)
LOCAL_CFLAGS += -DSTATS=$(if $(filter true,$(LINKER_STATS)),1,0)
LOCAL_CFLAGS += -DCOUNT_PAGES=$(if $(filter true,$(LINKER_COUNT_PAGES)),1,0)

LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk

ifeq ($(SINGLE_BINARY_SUPPORT),true)
  LOCAL_LDFLAGS := -shared -Wl,-Bsymbolic -Wl,-soname,olibc.so
  LOCAL_CFLAGS += -DOLIBC_SINGLE_BINARY_SUPPORT

  LOCAL_MODULE := olibc

  LOCAL_WHOLE_STATIC_LIBRARIES := libc_internal libm

  LOCAL_MODULE_CLASS := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := $(TARGET_SHLIB_SUFFIX)
  OVERRIDE_BUILT_MODULE_PATH := $(TARGET_OUT_INTERMEDIATE_LIBRARIES)

  LOCAL_SYSTEM_SHARED_LIBRARIES :=

  LOCAL_REQUIRED_MODULES := tzdata
else
  LOCAL_LDFLAGS := -shared -Wl,--exclude-libs,ALL -Wl,-Bsymbolic

    ifeq ($(TARGET_ARCH),$(filter $(TARGET_ARCH),x86_64))
      LOCAL_MODULE := linker64
    else
      LOCAL_MODULE := linker
    endif

  LOCAL_STATIC_LIBRARIES := libc_nomalloc

  #LOCAL_FORCE_STATIC_EXECUTABLE := true # not necessary when not including BUILD_EXECUTABLE

  #
  # include $(BUILD_EXECUTABLE)
  #
  # Instead of including $(BUILD_EXECUTABLE), we execute the steps to create an executable by
  # hand, as we want to insert an extra step that is not supported by the build system, and
  # is probably specific the linker only, so there's no need to modify the build system for
  # the purpose.

  LOCAL_MODULE_CLASS := EXECUTABLES
  LOCAL_MODULE_SUFFIX := $(TARGET_EXECUTABLE_SUFFIX)
endif

# we don't want crtbegin.o (because we have begin.o), so unset it
# just for this module
LOCAL_NO_CRT := true

# TODO: split out the asflags.
LOCAL_ASFLAGS := $(LOCAL_CFLAGS)

include $(BUILD_SYSTEM)/dynamic_binary.mk

# See build/core/executable.mk
$(linked_module): PRIVATE_TARGET_GLOBAL_LD_DIRS := $(TARGET_GLOBAL_LD_DIRS)
$(linked_module): PRIVATE_TARGET_GLOBAL_LDFLAGS := $(TARGET_GLOBAL_LDFLAGS)
$(linked_module): PRIVATE_TARGET_FDO_LIB := $(TARGET_FDO_LIB)
$(linked_module): PRIVATE_TARGET_LIBGCC := $(TARGET_LIBGCC)
$(linked_module): PRIVATE_TARGET_CRTBEGIN_DYNAMIC_O := $(TARGET_CRTBEGIN_DYNAMIC_O)
$(linked_module): PRIVATE_TARGET_CRTBEGIN_STATIC_O := $(TARGET_CRTBEGIN_STATIC_O)
$(linked_module): PRIVATE_TARGET_CRTEND_O := $(TARGET_CRTEND_O)

ifeq ($(SINGLE_BINARY_SUPPORT),true)
# Built as a shared library in single binary mode
$(linked_module): $(TARGET_CRTBEGIN_STATIC_O) $(all_objects) $(all_libraries) $(TARGET_CRTEND_O)
	$(transform-o-to-shared-lib)
else
$(linked_module): $(TARGET_CRTBEGIN_STATIC_O) $(all_objects) $(all_libraries) $(TARGET_CRTEND_O)
	$(transform-o-to-static-executable)
	@echo "target PrefixSymbols: $(PRIVATE_MODULE) ($@)"
	$(hide) $(TARGET_OBJCOPY) --prefix-symbols=__dl_ $@
endif

#
# end of BUILD_EXECUTABLE hack
#
