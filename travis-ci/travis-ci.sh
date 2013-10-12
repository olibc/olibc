#!/usr/bin/env bash
#

curl http://commondatastorage.googleapis.com/git-repo-downloads/repo > repo
chmod a+x ./repo

./repo init -u https://github.com/olibc/manifest.git
./repo sync
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


  make -j10 CONFIG=$config \
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
