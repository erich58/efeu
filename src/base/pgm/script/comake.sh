#!/bin/sh
# :*:calling make in corresponding build directory
# :de:Make im korriespondierenden build-Verzeichnis aufrufen
#
# $Copyright (C) 2008 Erich Frühstück
# This file is part of EFEU.
# 
# EFEU is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
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

# $pconfig
# Version=":VN:"
# t|
#	:*:run make in top directory
#	:de:make im Hauptverzeichnis aufrufen
# v|
#	:*:be verbose
#	:de:Einzelne Suchschritte auflisten
# n|
#	:*:Print  the  commands  that  would  be executed, but do not execute
#	them. This Option is passed through to make.
#	:de:Aktionen nur Anzeigen und nicht durchführen. Diese Option wird an
#	make weitergereicht.
# s:dir|
#	:*:run make in the subdirectory <dir> of the top directory.
#	Multiple use of this Option goes down the build hierarchy.
#	:de:make im Unterverzeichnis <sub> vom Hauptverzeichnis aufrufen.
#	Mehrfachverwendung dieser Option folgt der Aufbauhierarchie.
# ::make-arg(s) |
#	:*:any make argument
#	:de:beliebiges make-Argument

usage ()
{
	efeuman -- $0 $1 || cat << EOF
usage: $0 [-tvn] [-s sub] [--] [make-parameter]
EOF
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

# parse command args

use_top=
verbose=
subdir=
flags=

while getopts tvns: opt
do
	case $opt in
	t)	use_top=1;;
	v)	verbose=1;;
	s)	subdir="$subdir/$OPTARG";;
	n)	flags="$flags -n";;
	\?)	usage; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

top=`pwd`
list=
name=

while [ ! -d $top/build ]
do
	if [ "$top" = "/" ]; then
		echo "Top directory not found!"
		exit 1;
	fi

	list="$name $list"
	name=`basename $top`
	top=`dirname $top`
done

if [ "$name" != "src" ]; then
	echo "Aufruf außerhalb des src-Zweiges!"
	exit 1;
fi

[ $verbose ] && echo "Top Verzeichnis ist $top" >&2

if [ -n "$use_top" ]; then
	:
elif [ -n "$subdir" ]; then
	top=$top/build/$subdir
elif [ "$list" != " " ]; then
	top=$top/build

	for x in $list
	do
		if [ -f $top/$x/Makefile ]; then
			top=$top/$x
		elif [ -f $top/$x.d/Makefile ]; then
			top=$top/$x.d
		fi
	done
elif [ -f $top/build/Makefile ]; then
	top=$top/build
fi

cd $top || exit 1

# run make

if
	test -f Makefile
then
	[ $verbose ] && echo "Aufruf von make in `pwd`" >&2
	make $flags "$@"
else
	echo "`pwd`: Makefile nicht gefunden!"
	exit 1
fi
