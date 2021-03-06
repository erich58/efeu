#!/bin/sh
# :*: update vim configuration files for EFEU
# :de: Vim Konfigurationsdateien für EFEU erneuern
#
# $Copyright (C) 2002 Erich Frühstück
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
# :dir|
#	:*:directory with configuration files
#	:de:Verzeichnis mit Konfigurationsdateien

usage ()
{
	efeuman -- $0 $1 || echo "usage: $0 dir"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]'; exit 0;;
esac

# $SeeAlso
# vim(1).

if [ $# -ne 1 ]; then
	usage -?
	exit 1
fi

top=`dirname $0`

case "$top" in
	*/bin | bin)	top=`dirname $top`;;
esac

top=`cd $top; pwd`
cd $1 || exit 1
dir=`pwd`

Update ()
{
	if
		cmp -s $1 $2
	then
		echo "$1 is up to date"
		rm $2
	else
		echo "new version of $1 created"
		mv $2 $1
	fi
}

tmp=vimrc.$$
trap "rm -f $tmp" 0
trap "exit 1" 1 2 3

#	vimrc

printf '" configuration file created by %s, do not edit\n\n' "$0" > $tmp

find $dir -name '*.vimrc' | while read x
do
	printf "so %s\n" $x >> $tmp
done

printf "\nif has(\"syntax\")\n  syntax on\n" >> $tmp

find $dir/syntax -name '*.vim' | while read x
do
	y=`basename $x .vim`
	printf "  au Syntax %s so %s\n" $y $x >> $tmp
done

printf "endif\n\n" >> $tmp
printf 'set tags=./tags\\ %s/build/scope/tags\n' $top >> $tmp

if
	cmp -s vimrc $tmp
then
	echo "vimrc is up to date"
	rm $tmp
else
	echo "new version of vimrc created"
	mv $tmp vimrc
fi

exit 0
