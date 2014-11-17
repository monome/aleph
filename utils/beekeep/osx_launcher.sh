#!/bin/sh
name="`basename $0`"
tmp="$0"
tmp=`dirname "$tmp"`
bundle=`dirname "$tmp"`

bundle_contents="$bundle"/Contents
bundle_lib="$bundle_contents"/libs

export DYLD_LIBRARY_PATH="$bundle_lib"
export LD_LIBRARY_PATH="$DYLD_LIBRARY_PATH"

# Strip out the argument added by the OS.
if /bin/expr "x$1" : '^x-psn_' > /dev/null; then
    shift 1
fi

exec "$bundle_contents/MacOS/beekeep-bin"