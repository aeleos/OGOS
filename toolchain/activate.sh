#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. $DIR/config.sh

export PATH="$DIR/local/bin:$PATH"
export PKG_CONFIG_LIBDIR="$OGOS_SYSROOT/usr/lib/pkgconfig"
export PKG_CONFIG_SYSROOT_DIR="$OGOS_SYSROOT"
export TOOLCHAIN="$OGOS_SYSROOT/usr"
