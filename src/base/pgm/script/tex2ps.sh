#!/bin/sh
# :*:process TeX-document
# :de:TeX-Formatierung
#
# $Copyright (C) 1993, 2001 Erich Frühstück
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
# Version="$Id: tex2ps.sh,v 1.12 2009-05-15 20:15:46 ef Exp $"
# q |
#	:*:landscape
#	:de:Landscape
# n:count |
#	:*:number of formatting runs
#	:de:Zahl der Formatierungsläufe
# r:res |
#	:*:resolution
#	:de:Auflösung
# :src |
#	:*:source
#	:de:Definitionsfile
# ::ps |
#	:*:target
#	:de:Ausgabefile

usage ()
{
	efeuman -- $0 $1 || cat << EOF
usage: $0 [-q] [-n count] [-r res] src [ps]
EOF
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version="[$]Id:' $0; exit 0;;
esac

#	parse command line

name=TeX$$
texfile=$name.tex
dvifile=$name.dvi
count=1

while getopts pn:qr: opt
do
	case $opt in
	n)	count=$OPTARG;;
	q)	psflags="$psflags -t landscape";;
	r)	psflags="$psflags -D$OPTARG";;
	\?)	usage -?; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

if
	test $# -lt 1
then
	usage -?
	exit 1
fi

trap "rm -rf $name.*" 0
trap "exit 1" 1 2 3

#	create input file

if
	test A$1 = A-
then
	cat > $texfile
elif
	test -f $1.tex
then
	cp $1.tex $texfile
elif
	test -f $1
then
	cp $1 $texfile
else
	case $LANG in
	de*)	echo "$0: Datei \"$1\" nicht gefunden." >&2;;
	*)	echo "$0: file \"$1\" not found." >&2;;
	esac

	exit 1
fi


#	formatting with tex or latex

if
	fgrep -c documentstyle $texfile > /dev/null
then
	cmd=latex
elif
	fgrep -c documentclass $texfile > /dev/null
then
	cmd=latex
else
	cmd=tex
	echo "\\\\end" >> $texfile
fi

echo "formatting..." >&2

while [ $count -gt 0 ]
do
	$cmd $texfile < /dev/null >&2

	if [ -f $name.idx ]; then
		makeindex $name
	fi

	count=`expr $count - 1`
done

#	check for dvipost

if
	fgrep -c 'usepackage{dvipost}' $texfile > /dev/null
then
	dvipost $dvifile $dvifile
fi

#	converting

case `basename $0` in
	tex2ps)
		echo "converting..." >&2
		dvips $psflags $dvifile -o $name.ps
		outfile=$name.ps
		;;
	tex2pdf)
		echo "converting..." >&2
		dvips $psflags $dvifile -o $name.ps
		ps2pdf $name.ps $name.pdf
		outfile=$name.pdf
		;;
	*)	outfile=$dvifile
		;;
esac

#	copy output to target

if
	test $# -lt 2
then
	case `basename $0` in
	texshow)
		xdvi $outfile
		;;
	*)
		cat $outfile
		;;
	esac
elif
	test A$2 = A-
then
	cat $outfile
else
	cp $outfile $2
fi
