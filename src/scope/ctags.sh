#!/bin/sh
#determine ctags flags

TMP=`mktemp -d`
trap "rm -rf $TMP" 0
trap "exit 1" 1 2 3 15

cd $TMP
src=ctags_test.c
touch $src

if
	ctags -twa $src 2>/dev/null
then
	echo "ctags -twa"
elif
	ctags -wa $src 2>/dev/null
then
	echo "ctags -wa" 
fi
