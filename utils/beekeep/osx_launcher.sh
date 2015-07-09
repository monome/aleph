#!/bin/sh
name="`basename $0`"
tmp="$0"
tmp=`dirname "$tmp"`
<<<<<<< HEAD
tmp=`dirname "$tmp"`
bundle=`dirname "$tmp"`
bundle_contents="$bundle"/Contents
bundle_res="$bundle_contents"/Resources
bundle_lib="$bundle_res"/lib
bundle_bin="$bundle_res"/bin
bundle_data="$bundle_res"/share
bundle_etc="$bundle_res"/etc

export XDG_DATA_DIRS="$bundle_data"

export PATH="$bundle_contents"/MacOS:/bin:/usr/bin
export DYLD_LIBRARY_PATH="$bundle_lib"
export LD_LIBRARY_PATH="$DYLD_LIBRARY_PATH"

export GTK_DATA_PREFIX="$bundle_res"
export GTK_EXE_PREFIX="$bundle_res"
export GTK_PATH="$bundle_res"

export PANGO_RC_FILE="$bundle_etc/pango/pangorc"
export PANGO_SYSCONFDIR="$bundle_etc"
export PANGO_LIBDIR="$bundle_lib"

export GDK_PIXBUF_MODULE_FILE="$bundle_etc/gtk-3.0/gdk-pixbuf.loaders"
if [ `uname -r | cut -d . -f 1` -ge 10 ]; then
    export GTK_IM_MODULE_FILE="$bundle_etc/gtk-3.0/gtk.immodules"
fi
export GI_TYPELIB_PATH="$bundle_lib/girepository-1.0"

# Strip out the argument added by the OS.
if /bin/expr "x$1" : '^x-psn_' > /dev/null; then
    shift 1
fi

# aleph path
if [ -z ${ALEPH_DIR+x} ]
	then ALEPH_DIR=$HOME/aleph
fi
exec $bundle_contents/MacOS/beekeep-bin $ALEPH_DIR/
