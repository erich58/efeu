#!/bin/sh
# :*:run make in global building directory
# :de:make in globaler Generierungsbibliothek aufrufen
#
# Copyright (C) 1999 Erich Frühstück
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

# message formats

: ${LANG:=en}

fmt_usage="
usage: $0 [-hn] [-d dir] [-s sub] [target(s)]

-h	this output
-n	show rules, do not execute
-d dir	run make in <dir>
-s sub	run make in subdirectory <sub>.d

The command runs make in a global directory. This directory
is defined by the environment EFEUGEN. If EFEUGEN is not set,
\$EFEUTOP/build is used. If EFEUTOP is not set, \$HOME is substituted.
The current setting of EFEUGEN is \"$EFEUGEN\".

The option '-s' could be used more than once. A subdirectory name of
'a.b' is replaced by 'a.d/b.d' ('-s a.b' is equivalent to '-s a -s b').
"
fmt_norule="$0: missing Makefile in %s.\n"

case $LANG in
de*)
	fmt_usage="
Aufruf: $0 [-hn] [-d dir] [-s sub] [Ziele(e)]

-h	Dieser Text
-n	Regeln nur anzeigen
-d dir	make im Verzeichnis dir aufrufen
-s sub	make im Unterverzeichnis sub.d aufrufen

Das Kommando wechselt in das durch die Umgebungsvariable EFEUGEN
definierte Verzeichnis und ruft make
mit den angegebenen Argumenten auf.
Falls EFEUGEN nicht definiert ist, wird \$EFEUTOP/build genommen.

Die aktuelle Setzung von EFEUGEN ist \"$EFEUGEN\".

Die Option \"-s\" kann mehrfach verwendet
werden. Ein Unterverzeichnis der Form \"a.b\" wird in \"a.d/b.d\" konvertiert
(\"-s a.b\" ist damit äquivalent zu \"-s a -s b\").
"
	fmt_norule="$0: Kein Makefile in %s vorhanden.\n"
	;;
esac

# parse command args

top=${EFEUTOP:=$HOME}
gendir=${EFEUGEN:=$top/build}
flags=

while getopts d:s:nh opt
do
	case $opt in
	d)	gendir=$OPTARG;;
	s)	gendir="$gendir/"`echo $OPTARG | sed -e 's/\./\.d\//g'`".d";;
	n)	flags="$flags -n";;
	h)	echo "$fmt_usage"; exit 0;;
	\?)	echo "$fmt_usage" >&2; exit 1;;
	esac
done

shift `expr $OPTIND - 1`
cd $gendir || exit 1

# run make

if
	test -f Makefile -o -f makefile
then
	make $flags "$@"
else
	printf "$fmt_norule" `pwd`
fi
