#!/bin/sh


basedir=/usr/libexec

deps_dir=/usr/local/ks-scmc-gui
deps_lib_dir=$deps_dir/lib64
# export LD_LIBRARY_PATH=$deps_lib_dir:/usr/lib64:$LD_LIBRARY_PATH
export QT_QPA_PLATFORM_PLUGIN_PATH=$deps_lib_dir/plugins

exec "$basedir"/ks-scmc-gui
