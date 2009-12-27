#!/bin/sh
# :*:create system specific parameters in headerfiles
# :de:Systemspezifische Parameter für Headerdateien generieren
#
# $Copyright (C) 1999 Erich Frühstück
# This file is part of EFEU.
# 
# EFEU is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# EFEU is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public
# License along with EFEU; see the file COPYING.
# If not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

#	message formats

msg1="usage: $0 [-hv] [-c name] [-f flgs] cmd arg(s)\n"

case ${LANG:=en} in
de*)
	msg1="Aufruf: $0 [-hv] [-c name] [-f flgs] cmd arg(s)\n"
	;;
esac

tmp=/tmp/efeu$$
trap "rm -f $tmp*" 0
trap "exit 1" 1 2 3

CC=efeucc
flags=""
verbose=0
incl=""

#	command usage

usage ()
{
	printf "$msg1"

	case $LANG in
	de*)
	cat << EOF

	-h	Dieser Text
	-v 	Fehlermeldungen ausgeben
	-c name	C-Kompiler zum Testen
	-f flgs	Flags für C-Kompiler

	typedef <type> <decl>
		Falls der Type <type> nicht deklariert ist, wird
		er mit <decl> <type> definiert.
	func <tname> <expr>
		Falls der Aufruf <expr> erfolgreich ist, wird der
		Makro <tname> mit 1, ansonsten mit 0 definiert.
	byteorder
		Je nach Byteordnung wird der Makro <REVBYTEORDER>
		auf 1 oder 0 gesetzt.
	proto <proto>
		Testet die Kompatiblität des Prototypes <proto>.
EOF
		;;
*)
		cat << EOF
	-h	display this help and exit
	-v 	dosplay error notes
	-c name	C-Compiler
	-f flgs	Compiler flags

	typedef <type> <decl>
		if <type> is not defined, declare it with <decl>.
	func <tname> <expr>
		if <expr> is successful, define <tname> with 1,
		else with 0.
	byteorder
		set the Macro REVBYTEORDER corresponding to the
		byteorder of the system.
	proto <proto>
		check compatibility of prototype <proto>.
EOF
		;;
	esac
	exit 0
}

#	parse command args

while getopts "hvc:f:i:" opt
do
	case $opt in
	h)	usage; exit 0;;
	v)	verbose=1;;
	c)	CC="$OPTARG";;
	f)	flags="$OPTARG";;
	i)	incl="$incl
#include <$OPTARG>";;
	\?)	printf "$msg1"; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

if	[ $# -lt 1 ]
then	printf "$msg1"; exit 1
fi

decl=""
body=""
success=":"
failure=":"
type=$1
shift

#	Test bestimmen

case $type in
typedef)
	name=$1; shift
	failure="echo typedef $* $name;"
	body="$name x;"
	;;
func)
	success="echo #define $1 1"
	failure="echo #define $1 0"
	body="$2;"
	;;
byteorder)
	success="$tmp"
	failure="echo #error Byteorder not identified."
	body='int x = 1;
printf("#undef	REVBYTEORDER\n");
printf("#define	REVBYTEORDER %d\n", ((char *) &x)[0]);'
	;;
proto)
	success="echo $*;"
	failure="echo prototype not compatible >&2"
	decl="$*;"
	;;
test)
	success="echo success"
	failure="echo failure"
	body="$*;"
	;;
esac

#	run test

cat > $tmp.c <<!
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
$incl
$decl

int main (void)
{
$body
return 0;
}
!

if	$CC $flags -o $tmp $tmp.c 2> $tmp.log
then	$success
else	$failure
fi

if	[ $verbose -gt 0 ]; then
	cat $tmp.c >&2
	cat $tmp.log >&2
fi

rm -f $tmp.c $tmp.s $tmp.log
