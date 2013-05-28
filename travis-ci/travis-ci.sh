#!/usr/bin/env bash
#

curl https://dl-ssl.google.com/dl/googlesource/git-repo/repo > repo
chmod a+x ./repo

./repo init -u https://github.com/olibc/manifest.git
./repo sync
echo "OLIBC_CONF:=.config-olibc" > Makefile
echo "-include \$(OLIBC_CONF)" >> Makefile
echo "include build/core/olibc.mk" >> Makefile
echo "include build/core/main.mk" >> Makefile
echo "build/core/BUILD-real.sh \$@" > BUILD.sh
chmod +x BUILD.sh
for config in configs/*
do
  x=`grep EXT_MALLOC_LEAK_CHECK=true $config`

  if [ "$x" !=  '' ]
  then
    MALLOC_LEAK=libc_malloc_debug_leak
  else
    MALLOC_LEAK=
  fi

  x=`grep EXT_MALLOC_ANDROID_QEMU_INSTRUMENT=true $config`

  if [ "$x" !=  '' ]
  then
    MALLOC_QEMU=libc_malloc_debug_qemu
  else
    MALLOC_QEMU=
  fi


  ./BUILD.sh -j10 OLIBC_CONF=$config \
             bionic-unit-tests-static bionic-unit-tests \
             bionic-benchmarks \
             $MALLOC_LEAK \
             $MALLOC_QEMU

  rc=$?
  if [[ $rc != 0 ]] ; then
      echo "Build error on $config"
      exit $rc
  fi
done
