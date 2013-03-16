_LIBC_ARCH_COMMON_SRC_FILES := \
    arch-arm/bionic/abort_arm.S \
    arch-arm/bionic/atomics_arm.c \
    arch-arm/bionic/clone.S \
    arch-arm/bionic/eabi.c \
    arch-arm/bionic/_exit_with_stack_teardown.S \
    arch-arm/bionic/ffs.S \
    arch-arm/bionic/futex_arm.S \
    arch-arm/bionic/__get_sp.S \
    arch-arm/bionic/kill.S \
    arch-arm/bionic/libgcc_compat.c \
    arch-arm/bionic/memcmp16.S \
    arch-arm/bionic/memcmp.S \
    arch-arm/bionic/_setjmp.S \
    arch-arm/bionic/setjmp.S \
    arch-arm/bionic/sigsetjmp.S \
    arch-arm/bionic/strcmp.S \
    arch-arm/bionic/strcpy.S \
    arch-arm/bionic/syscall.S \
    arch-arm/bionic/tgkill.S \
    arch-arm/bionic/tkill.S \

ifeq ($(ARCH_ARM_HAVE_ARMV7A),false)
_LIBC_ARCH_COMMON_SRC_FILES += arch-arm/bionic/strlen-armv7.S
else
_LIBC_ARCH_COMMON_SRC_FILES += arch-arm/bionic/strlen.c.arm
endif

# Check if we want a neonized version of memmove instead of the
# current ARM version
ifeq ($(TARGET_USE_SCORPION_BIONIC_OPTIMIZATION),true)
_LIBC_ARCH_COMMON_SRC_FILES += \
	arch-arm/bionic/memmove.S
else
ifneq (, $(filter true,$(TARGET_USE_KRAIT_BIONIC_OPTIMIZATION) $(TARGET_USE_SPARROW_BIONIC_OPTIMIZATION)))
 _LIBC_ARCH_COMMON_SRC_FILES += \
	arch-arm/bionic/memmove.S
 else # Other ARM
 _LIBC_ARCH_COMMON_SRC_FILES += \
	string/bcopy.c \
	bionic/memmove.c.arm
 endif # !TARGET_USE_KRAIT_BIONIC_OPTIMIZATION
endif # !TARGET_USE_SCORPION_BIONIC_OPTIMIZATION

# These are used by the static and dynamic versions of the libc
# respectively.
_LIBC_ARCH_STATIC_SRC_FILES := \
    arch-arm/bionic/exidx_static.c

_LIBC_ARCH_DYNAMIC_SRC_FILES := \
    arch-arm/bionic/exidx_dynamic.c

ifeq ($(strip $(wildcard bionic/libc/arch-arm/$(TARGET_CPU_VARIANT)/$(TARGET_CPU_VARIANT).mk)),)
$(error "TARGET_CPU_VARIANT not set or set to an unknown value. Possible values are cortex-a9, cortex-a15, krait. Use generic for devices that do not have a CPU similar to any of the supported cpu variants.")
endif

include bionic/libc/arch-arm/$(TARGET_CPU_VARIANT)/$(TARGET_CPU_VARIANT).mk
