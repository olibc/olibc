
ifeq ($(STPCPY_IMPL),true)
libc_bionic_src_files += \
	string/stpcpy.c \
	string/stpncpy.c \

endif

ifeq ($(RAWMEMCHR_IMPL),true)
libc_bionic_src_files += \
	string/rawmemchr.c \

endif

ifeq ($(INSQUE_REMQUE_IMPL),true)
libc_bionic_src_files += \
	bionic/insque.c \
	bionic/remque.c
endif

ifeq ($(SHADOW_PASSWD_FILE_IMPL),true)
libc_bionic_src_files += \
	passwd/fgetspent.c \
	passwd/fgetspent_r.c \
	passwd/getspent.c \
	passwd/getspent_r.c \
	passwd/getspnam.c \
	passwd/getspnam_r.c \
	passwd/lckpwdf.c \
	passwd/sgetspent.c \
	passwd/sgetspent_r.c \
	passwd/putspent.c \
	passwd/fgetpwent.c \
	passwd/fgetpwent_r.c \
	passwd/getpwent.c \
	passwd/getpwent_r.c \
	passwd/getpwnam.c \
	passwd/getpwnam_r.c \
	passwd/getpwuid.c \
	passwd/getpwuid_r.c \
	passwd/putpwent.c \
	passwd/fgetgrent.c \
	passwd/fgetgrent_r.c \
	passwd/getgrent.c \
	passwd/getgrent_r.c \
	passwd/getgrgid.c \
	passwd/getgrgid_r.c \
	passwd/getgrnam.c \
	passwd/getgrnam_r.c \
	passwd/putgrent.c \
	passwd/initgroups.c \
	passwd/getgrouplist.c \
	passwd/passwd_private.c \

else

libc_bionic_src_files += bionic/stubs.c

endif

ifeq ($(A64L_L64A_IMPL),true)
libc_bionic_src_files += \
	bionic/a64l.c \
	bionic/l64a.c \

endif


ifeq ($(MEMPCPY_IMPL),true)
libc_bionic_src_files += \
	bionic/mempcpy.c \

  ifeq ($(WCHAR_SUPPORT),true)
	libc_bionic_src_files += \
	  wchar/wmempcpy.c
  endif

endif

ifeq ($(STRCHRNUL_IMPL), true)
libc_bionic_src_files += \
	string/strchrnul.c

endif


ifeq ($(MNTENT_SUPPORT), true)
libc_bionic_src_files += \
	bionic/addmntent.c \
	bionic/getmntent_r.c \
	bionic/getmntent.c \
	bionic/endmntent.c \
	bionic/hasmntopt.c \
	bionic/setmntent.c \


endif

ifeq ($(MD5_IMPL),true)
libc_common_no_strict_alias_src_files += \
	bionic/md5.c

libc_common_cflags := \
    -DMD5_IMPL
endif

ifeq ($(CRYPT_IMPL),true)
libc_bionic_src_files += \
	unistd/crypt.c \
	unistd/des_crypt.c \

endif

ifeq ($(SHA1_IMPL),true)
libc_upstream_netbsd_src_files += \
    upstream-netbsd/common/lib/libc/hash/sha1/sha1.c
endif

ifeq ($(LSEARCH_LFIND_IMPL),true)
libc_upstream_netbsd_src_files += \
    upstream-freebsd/lib/libc/stdlib/lsearch.c
endif

ifeq ($(WCHAR_SUPPORT),true)
libc_bionic_src_files += \
    bionic/wcscoll.c \
    wchar/wcswidth.c \
    wchar/wcsxfrm.c \

libc_bionic_src_files += \
    bionic/wchar.c

libc_upstream_freebsd_src_files += \
    upstream-freebsd/lib/libc/string/wcpcpy.c \
    upstream-freebsd/lib/libc/string/wcpncpy.c \
    upstream-freebsd/lib/libc/string/wcscasecmp.c \
    upstream-freebsd/lib/libc/string/wcscspn.c \
    upstream-freebsd/lib/libc/string/wcsdup.c \
    upstream-freebsd/lib/libc/string/wcslcat.c \
    upstream-freebsd/lib/libc/string/wcslcpy.c \
    upstream-freebsd/lib/libc/string/wcsncasecmp.c \
    upstream-freebsd/lib/libc/string/wcsncat.c \
    upstream-freebsd/lib/libc/string/wcsncmp.c \
    upstream-freebsd/lib/libc/string/wcsncpy.c \
    upstream-freebsd/lib/libc/string/wcsnlen.c \
    upstream-freebsd/lib/libc/string/wcspbrk.c \
    upstream-freebsd/lib/libc/string/wcsspn.c \
    upstream-freebsd/lib/libc/string/wcsstr.c \
    upstream-freebsd/lib/libc/string/wcstok.c \
    upstream-freebsd/lib/libc/string/wmemchr.c \
    upstream-freebsd/lib/libc/string/wmemcpy.c \
    upstream-freebsd/lib/libc/string/wmemmove.c \
    upstream-freebsd/lib/libc/string/wmemset.c \


  ifneq ($(TARGET_ARCH),x86)
    libc_upstream_freebsd_src_files += \
      upstream-freebsd/lib/libc/string/wcscat.c \
      upstream-freebsd/lib/libc/string/wcschr.c \
      upstream-freebsd/lib/libc/string/wcscmp.c \
      upstream-freebsd/lib/libc/string/wcslen.c \
      upstream-freebsd/lib/libc/string/wcsrchr.c \
      upstream-freebsd/lib/libc/string/wmemcmp.c \
      upstream-freebsd/lib/libc/string/wcscpy.c \

  endif

endif

ifneq ($(TARGET_ARCH),arm)
  ifeq ($(SYSVIPC_SUPPORT),true)
    libc_bionic_src_files += \
      sysvipc/semctl.c \
      sysvipc/semget.c \
      sysvipc/semtimedop.c \
      sysvipc/semop.c \
      sysvipc/shmat.c \
      sysvipc/shmctl.c \
      sysvipc/shmdt.c \
      sysvipc/shmget.c \
      sysvipc/msgctl.c \
      sysvipc/msgsnd.c \
      sysvipc/msgrcv.c \
      sysvipc/msgget.c
  endif
endif


ifeq ($(PROPERTY_SYSTEM_SUPPORT),true)
  libc_bionic_src_files += \
	bionic/system_properties_compat.c \
	bionic/system_properties.c
  libc_common_cflags += -DPROPERTY_SYSTEM_SUPPORT
endif

ifeq ($(LOGGER_SYSTEM_SUPPORT),true)
  libc_bionic_src_files += \
	bionic/libc_logging.c
  libc_common_cflags += -DLOGGER_SYSTEM_SUPPORT
else
  libc_bionic_src_files += \
	bionic/libc_logging_stderr.c
endif

ifeq ($(UTMPX_SUPPORT),true)
  libc_bionic_src_files += \
	bionic/utmpx.c
  libc_common_cflags += -DUTMPX_SUPPORT
endif
