#!/bin/sh

if [ -n "$KBUILD_DIR" ]; then
    make -C $KBUILD_DIR M=$(pwd) $*
else
    make -C /lib/modules/$(uname -r)/build M=$(pwd) $*
fi
