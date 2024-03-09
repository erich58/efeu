#!/bin/sh
# :*:update rule for configuration files
# :de:Update-Regel für Konfigurationsdateien
#
# $Copyright (C) 2019 Erich Frühstück
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

msg1="usage: $0 [--help] [opt] tg cmd [args]\n"

case ${LANG:=en} in
de*)
	msg1="Aufruf: $0 [--help] [opt] tg cmd [args]\n"
	;;
esac

# $pconfig
# Version=":VN:"
# -v |
#	:*:show what is being done
#	:de:Durchgeführte Tätigkeiten anzeigen
# :tg|
#	:*:target file
#	:de:Zieldatei
# :cmd|
#	:*:command to create target
#	:de:Befehl zum generieren der Zieldatei
# *args|
#	:*:extra arguments to create target
#	:de:Zusätzliche Argumente für Zieldatei

usage ()
{
	efeuman -- $0 $1 || printf "$msg1"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

verbose=""

while getopts "v" opt
do
	case $opt in
	v)	verbose=true;;
	\?)	usage -?; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

if	[ $# -lt 2 ]
then	usage -?; exit 1
fi

tmp=`mktemp`
trap "rm -vf $tmp" 0
trap "exit 1" 1 2 3 15 

tg=$1
shift 1
mkdir -p `dirname $tg` || exit 1
"$@" > $tmp

if	[ -f $tg ] && cmp -s $tg $tmp
then
	rm $tmp
	[ "$verbose" ] && echo "$tg is up to date"
else
	mv $tmp $tg
	echo "$@ > $tg"
fi

exit 0
