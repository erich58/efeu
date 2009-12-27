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
# x|
#	:*:consider source generation with |mksource|.
#	:de:Berücksichtige Sourcgenerierung mit |mksource|.
# e:pat|
#	:*:ignore files which maches pattern <pat>
#	:de:Dateien ignorieren, die dem Muster <pat> entsprechen
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

case $LANG in
de*)
	fmt_keep="$0: %s wurde nicht verändert\n"
	fmt_modify="$0: %s wurde verändert\n"
	fmt_create="Sourceliste %s wird generiert\n"
	;;
esac

# cleanup rules

tmp=${TMPDIR:-/tmp}/sl$$
trap "rm -f $tmp.*" 0
trap "exit 1" 1 2 3

# create exception list

cat > $tmp.except <<!
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
\|\.o$|d
\|\.a$|d
\|\.so$|d
!

# parse command args

force=0
gensrc=0

while getopts hfxe:o: opt
do
	case $opt in
	f)	force=1;;
	x)	gensrc=1;;
	e)	echo "$OPTARG" | sed -e 's/\./\\&/g' \
			-e 's/^.*$/\\|&|d/' >> $tmp.except;;
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

top=`(cd $1 || exit 1; pwd)` || exit 1

# :*:remove / on end of directorys name for correct interpretaion of
# exeption list.
# :de:Ein / am Ende eines Bibliotheksnamens muß entfernt werden,
# da ansonsten die Ausnahmefiles nicht korrekt interpretiert
# werden.

for x in `find $top -type d -print | sed -e 's|/$||'`
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

# :*:create file list
# :de:Fileliste generieren

find $top -depth -type f -print | sed -f $tmp.except > $tmp.files
rm -f $tmp.except

if	[ $gensrc != 0 ]; then
	grep '\.tpl$' $tmp.files > $tmp.new
	sed -e 's/.tpl$//' -e 's/^.*$/\\|^&\\.|d/' $tmp.new > $tmp.except
	sed -f $tmp.except $tmp.files >> $tmp.new
	mv $tmp.new $tmp.files
	rm -f $tmp.except
fi

# :*:sort file list by depth !
# This is done by creating the depth information with awk and
# removing it with cut after sort.
# :de:Fileliste generieren und nach der Tiefe sortieren !!!
# Die Tiefe wird mit einem awk-Skript dem Filenamen
# vorangestellt und nach der Sortierung mit cut entfernt.
	
awk -F/ '{ printf("%03d %s\n", NF, $0) }' $tmp.files |\
sort | eval cut -c 5- $ofile
