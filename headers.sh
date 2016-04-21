#!/bin/sh
set -e
. ./config.sh


for PROJECT in $SYSTEM_HEADER_PROJECTS; do
  DESTDIR="$PWD/sysroot" $MAKE -C $PROJECT install-headers
done
