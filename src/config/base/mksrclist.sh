#!/bin/sh
# :*:create source list
# :de:Sourecliste generieren
#
# $Copyright (C) 2000 Erich Frühstück
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
# f|
#	:*:force creation of output file
#	:de:Forcierte Neugenerierung der Ausgabedatei
# :top|
#	:*:top directory of sources
#	:de:Hauptbibliothek der Sourcen
# ::name|
#	:*:name of output file
#	:de:Name der Ausgabedatei

# $SeeAlso
# dir2make(1).

usage ()
{
	efeuman -- $0 $1 || echo "usage: $0 [-f] top [name]"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
esac

# message formats

: ${LANG:=en}

fmt_keep="$0: %s keeped unchanged\n"
fmt_modify="$0: %s was modified\n"
fmt_create="create sourcelist %s\n"
fmt_nosrc="source directory %s does not exist\n"

case $LANG in
de*)
	fmt_keep="$0: %s wurde nicht verändert\n"
	fmt_modify="$0: %s wurde verändert\n"
	fmt_create="Sourceliste %s wird generiert\n"
	fmt_nosrc="Sourcebibliothek %s existiert nicht\n"
	;;
esac

# cleanup rules

tmp=${TMPDIR:-/tmp}/sl$$
trap "rm -f $tmp*" 0
trap "exit 1" 1 2 3

# parse command args

force=0

while getopts hfo: opt
do
	case $opt in
	f)	force=1;;
	o)	name=$OPTARG;;
	\?)	usage; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

# determine output

update ()
{
	cat > $tmp.lst

	if
		cmp -s $tmp.lst $1
	then
		printf "$fmt_keep" $1
		rm $tmp.lst
		exit 0
	else
		printf "$fmt_modify" $1
		mv $tmp.lst $1
		exit 1
	fi
}

if
	test $# -eq 2
then
	if
		test $force -eq 0 -a -f $2
	then
		ofile="|update $2"
	else
		printf "$fmt_create" $2
		ofile=" > $2"
	fi
elif
	test $# -eq 1
then
	ofile=""
else
	usage
	exit 1
fi

# check source directory

if
	test ! -d $1
then
	printf "$fmt_nosrc" $1
	exit 1
fi

# create exception list

filter ()
{
	sed -e 's/[./]/\\&/g' -e 's|^.*$|/^&/d|' $1
}

cat >> $tmp.except <<!
\|~|d
\|/old/|d
\|/unsup/|d
\|/save/|d
\|/CVS/|d
\|/core$|d
\|/LOCALFILES$|d
\|/READ.*ME$|d
\|/TODO$|d
\|/\.|d
\|\.swp$|d
!

# :*:remove / on end of directorys name for correct interpretaion of
# exeption list.
# :de:Ein / am Ende eines Bibliotheksnamens muß entfernt werden,
# da ansonsten die Ausnahmefiles nicht korrekt interpretiert
# werden.

for x in `find $1 -type d -print | sed -e 's|/$||'`
do
	if
		test -f $x/LOCALFILES
	then
		sed -e '/^[ 	]*#/d' -e '/^[ 	]*$/d'	\
			-e "s|^[ 	]*|$x/|" \
			-e 's/\./\\&/g' -e 's/^.*$/\\|^&|d/' \
			$x/LOCALFILES >> $tmp.except
	fi
done

# :*:create file list and sort by depth !
# This is done by creating depth with awk and removing ist
# with cut after sort.
# :de:Fileliste generieren und nach der Tiefe sortieren !!!
# Die Tiefe wird mit einem awk-Skript dem Filenamen
# vorangestellt und nach der Sortierung mit cut entfernt.
	
find $1 -depth -type f -print | sed -f $tmp.except |\
	awk -F/ '{ printf("%03d %s\n", NF, $0) }' |\
	sort | eval cut -c 5- $ofile
