#!/bin/sh


basedir=/usr/libexec

export LD_LIBRARY_PATH="LD_LIBRARY_PATH:/opt/ks-scmc/usr/lib64"

exec "$basedir"/ks-scmc-gui
