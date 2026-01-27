#!/bin/sh

flatpak-spawn --host which fusermount > /dev/null 2>&1
if [ $? -eq 0 ]; then
    FUSERMOUNT=fusermount
else
    FUSERMOUNT=fusermount3
fi
if [ -z "$_FUSE_COMMFD" ]; then
    FD_ARGS=
else
    FD_ARGS="--env=_FUSE_COMMFD=${_FUSE_COMMFD} --forward-fd=${_FUSE_COMMFD}"
fi

exec flatpak-spawn --host $FD_ARGS $FUSERMOUNT "$@"
