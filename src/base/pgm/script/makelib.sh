#!/bin/sh
#	Bibliothek auf neuesten Stand bringen

tmp=/usr/tmp/up$$


if
	test $# -ne 1
then
	echo "Aufruf: $0 Bibliothek"
	exit 1
fi

if
	test -f $1
then
	find *.o -newer $1 -print > $tmp
else
	find *.o -print > $tmp
fi

n=`wc -l < $tmp`

if
	test $n -gt 100
then
	rm -f $1
	ar qv $1 *.o
	ranlib $1
elif
	test $n -gt 0
then
	ar rv $1 `cat $tmp`
	ranlib $1
else
	echo "$1 is up to date"
fi

rm -f $tmp
