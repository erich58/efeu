#!/bin/sh

magic=`efeutop`/etc/magic

if [ -f $magic ]; then
	exec file -m $magic "$@"
else
	exec file "$@"
fi
