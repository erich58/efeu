#!/bin/sh
# :*:recursiv call of sed for all files in a directory
# :de:sed rekursiv auf alle Dateien eines Verzeichnisses anwenden
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
# Version="$Id: rsed.sh,v 1.1 2008-03-02 15:49:51 ef Exp $"
# d|
#	:*:show differences after editing without replacing
#	the orignial file.
#	:de:Zeige Unterschiede nach den Änderungen ohne die
#	Originaldatei zu verändern.
# c|
#	:*:same as |-d|, but use contect diff
#	:de:Wie Option -d, aber mit Kontextvergleich.
# name:pat|
#	:*:edit only files which match pattern <pat> (find option).
#	:de:Ändere nur Dateien, die dem Muster <pat> entsprechen
#	(|find| Option).
# ::dir|
#	:*:directories
#	:de:Verzeichnisse
# :"sed-options"|
#	:*:any sed option
#	:de:beliebige sed - Option

usage ()
{
	efeuman -- $0 $1 || echo "usage: $0 [-dc] dir(s) sed-options"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version="[$]Id:' $0; exit 0;;
esac

# $Description
# :*:
# The command |\$!| runs |sed| on all files collected by |find|.
# :de:
# Das Kommando |\$!| wendet |sed| auf alle mit |find| zusammengestellte
# Dateien an.
#
# @arglist -i

# $SeeAlso
# sed(1), find(1).

# parse rsed specific args

cmd="cp"
list=""
flags=""

while [ $# -gt 0 ]
do
	case $1 in
	-d)	cmd="diff"; shift 1;;
	-c)	cmd="diff -c"; shift 1;;
	-name)	flags="$flags $1 \"$2\""; shift 2;;
	--)	shift 1; break;;
	-*)	break;;
	*)	list="$list $1"; shift 1;;
	esac
done

#	check number of arguments

if [ $# -lt 1 ]; then
	usage -?
	exit 1
fi

# check sed on /dev/null

sed "$@" /dev/null || exit 1

tmp=${TMPDIR:-/tmp}/rsed$$
trap "rm -f $tmp" 0
trap "exit 1" 1 2 3

eval find $list -type f $flags -print | while read name
do
	sed "$@" "$name" > $tmp

	if
		cmp -s $tmp $name
	then
		continue
	fi

	echo $name
	$cmd $tmp $name
	rm -f $tmp
done
