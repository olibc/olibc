$(call libc-add-cpu-variant-src,MEMCPY,arch-arm/krait/bionic/memcpy.S)
$(call libc-add-cpu-variant-src,MEMSET,arch-arm/krait/bionic/memset.S)
$(call libc-add-cpu-variant-src,MEMMOVE,arch-arm/krait/bionic/memmove.S)
# bcopy implemented in memmove.S
$(call libc-add-cpu-variant-src,BCOPY,)

include bionic/libc/arch-arm/generic/generic.mk
