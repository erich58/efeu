#!/bin/sh
# :*:compare directoriies
# :de:Verzeichnisse vergleichen
#
# $Copyright (C) 2001 Erich Frühstück
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
# l|
#	:*:show all different characters (cmp -l)
#	:de:Ausgabe aller abweichenden Bytes (cmp -l)
# b|
#	:*:Ignore changes in the amount of white space.
#	:de:Weissen Zeichen beim Vergleich ignorieren.
# d|
#	:*:use |diff| instead of |cmp|
#	:de:Vergleich mit |diff| anstelle von |cmp|
# c|
#	:*:use |diff -c| instead of |cmp|
#	:de:Kontextvergleich mit |diff| anstelle von |cmp|
# v|
#	:*:list all performed comparisons
#	:de:Protokoll der durchgeführten Vergleiche
# :dir1|
#	:*:first directory
#	:de:1. Verzeichnis
# :dir2|
#	:*:second directory
#	:de:2. Verzeichnis

usage ()
{
	efeuman -- $0 $1 || echo "usage: $0 [-ldcv] dir1 dir2"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

# $Description
# :*:
# The command |\$!| compares files in <dir1> with files in <dir2>.
# The processing is recursive. The compareson is not symetric: it's
# okay for <dir1> to contain less files than <dir2>.
# :de:
# Das Kommando |\$!| vergleicht Dateien in <dir1> mit Dateien in <dir2>.
# Die Verarbeitung erfolgt rekursiv. Der Vergleich ist nicht symetrisch:
# Es ist in Ordnung, wenn <dir1> weniger Dateien als <dir2> enthält.
#
# @arglist -i
#
# $Examples
# :*:The following lines shoes the usage of |\$!|:
# :de:Die folgenden Befehlszeilen zeigen die Verwendung von |\$!:|
#
#	|mkdir save|\br
#	|cp| <files> |save|\br
#	...\br
#	|dircmp save .|

# $SeeAlso
# cmp(1), diff(1), dircpy(1).

# message formats

: ${LANG:=en}

fmt_noex="$0: directory %s does not exist.\n"

case $LANG in
de*)
	fmt_noex="$0: Verzeichnis %s existiert nicht.\n"
	;;
esac

# parse command args

pgm=cmp
popt=
verbose=0

while getopts bcdlv opt
do
	case $opt in
	b)	pgm="diff"; popt="$popt -b";;
	c)	pgm="diff"; popt="$popt -c";;
	d)	pgm="diff";;
	l)	pgm="cmp -l"; popt=""; verbose=1;;
	v)	verbose=1;;
	\?)	usage -?; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

if [ $# -ne 2 ]; then
	usage -?
	exit 1
elif [ ! -d $1 ]; then
	printf "$fmt_noex" $1
	exit 1
elif [ ! -d $2 ]; then
	printf "$fmt_noex" $2
	exit 1
fi

source=$1
target=$2
shift 2

( cd $source; find . -type f "$@" -print) | sed -e '/[/]CVS[/]/d' | while read x
do
	if [ $verbose -gt 0 ]; then
		echo $pgm $popt "$source/$x" "$target/$x"
	fi

	$pgm $popt "$source/$x" "$target/$x"
done
