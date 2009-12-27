#!/bin/sh
# :*:create Makefile from Imakefile, cpp version
# :de:Makefile aus Imakefile generieren
#
# $Copyright (C) 1999 Erich Frühstück
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
#	A-3423 St. Andrä/Wördern, Südtirolergasse 17-21/5

# $pconfig
# Version="$Id: ccmkmf.sh,v 1.6 2008-04-06 18:16:31 ef Exp $"
# n |
#	:*:no filter
#	:de:Keinen Ausgabefilter
# p |
#	:*:plain filter
#	:de:Plain-Filter
# g |
#	:*:use GNU - preprocessor (ANSI)
#	:de:GNU - Preprozessor (ANSI) verwenden
# G |
#	:*:use GNU - preprocessor (traditional)
#	:de:GNU - Preprozessor (traditionell) verwenden
# c:name |
#	:*:configuration file
#	:de:Konfigurationsdatei
# d:target |
#	:*:create dependence rule for target
#	:de:Abhängigkeitsregel für target generieren
# I:dir |
#	:*:expand include path with dir
#	:de:Suchpfad um dir erweitern
# U:name |
#	:*:delete macro name
#	:de:Makro name löschen
# D:name=val |
#	:*:define macro name
#	:de:Makro name definieren
# :src |
#	:*:source file (default Imakefile)
#	:de:Definitionsfile (default Imakefile)
# :tg |
#	:*:target file (default Makefile)
#	:de:Zieldatei (default Makefile)

usage ()
{
	efeuman -- $0 $1 || cat << EOF
usage: $0 [-npgG] [-c cfg] [-d target] [-I dir]
	[-U name] [-D name[=val]] file
EOF
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version="[$]Id:'; exit 0;;
esac

# $Description
# :*:
# The command |$1| creates a Makefile from Imakefile similar to
# |xmkmf| (X11 development).
# This version uses |cpp| as preprocessor and |ppfilter| as post filter.
# The following options and arguments are accepted from command |$!|:
# :de:
# Das Kommando |$!| generiert ein Makefile aus einem Imakefile analog zu
# xmkmf (X11 development).
# Diese Version benutzt |cpp| als Präprozessor und |ppfilter| als
# Postfilter.
# Folgende Optionen und Argumente werden vom Kommando |$!| akzeptiert:
#
# @arglist
#
# :*:
# For normal use, |mkmf| is called without arguments.
# Most of the flags are only used for compatibility checks.
# :de:
# Im Normalfall benötigt |mkmf| keine Optionen.
# Die meisten Flags dienen Kompatiblitätstests.

# $Description
# :de:
# \shead Probleme mit dem C-Preprozessor
# Die einzelnen Befehlszeilen in Regeln müssen mit einem Tabulator
# eingerückt sein, dieser darf nicht durch Leerzeichen ersetzt werden.
# Für C sind aber Tabulatoren und Leerzeichen völlig gleichwertig.
# \par
# Innerhalb von Makroersetzungen wurden schon jetzt Tabulatoren in
# Leerzeichen ersetzt. Daher wird in Makrodefinitionen die
# Zeichenkombinattion |@+| eingesetzt, die von einem nachgeschaltenen
# Filter in einen Tabulator umgewandelt wird.
# \par
# Es gibt aber auch Preprozessoren, die auch Tabulatoren außerhalb von
# Makrodefinitionen in Leerzeichen umwandeln. Hier müssten auch im
# Imakefile selbst alle Tabulatoren durch |@+| ersetzt werden.
# \par
# Im Zuge der Umstellung des GNU-C Komilers von ISO/IEC 9899:1990 auf
# ISO/IEC 9800:1999 (ANSI C) wurde die Ausgabe des C-Preprozessors
# geändert. Ab Version gcc-3.0 werden Tabulatoren in Leerzeichen
# umgewandelt. Durch Rückgriff auf den traditionellen Preprozesser (K\&R C)
# kann dieses Problem vorerst noch abgefangen werden.

# $Notes
# :*:This command is deprecated.
# :de: Das Kommando ist veraltet.

# $SeeAlso
# dir2make(1), ppfilter(1), pp2dep(1).

# message formats

: ${LANG:=en}

fmt_nodef="$0: file %s not readable\n"
fmt_hdr="$0: insert header %s.\n"

case $LANG in
de*)
	fmt_nodef="$0: Definitionsfile %s nicht lesbar\n"
	fmt_hdr="$0: Headerdatei %s eingefügt.\n"
	;;
esac

# setup parameters und filters

top=`dirname $0`

case "$top" in
	*/bin)	top=`dirname $top`;;
esac

echo $magic
CC="efeucc"
pathcfg="$top/etc/mkmf.cfg"
defcfg="mkmf.cfg"
cfg=""

if	gcc --version >/dev/null 2>&1; then
	CC="gcc -traditional-cpp"
fi

if
	ldef=`expr "$LANG" : '\([a-z][a-z]\).*'`
then
	ldef="-DLANG_$ldef"
fi

ppfilter="$top/bin/ppfilter"
pp2dep="$top/bin/pp2dep"
flags="-DEFEUROOT=$top $ldef"
tmp=${TMPDIR:-/tmp}/pp$$.c
if=Imakefile
of=Makefile
id=$0
bootstrap="$@"
stdinc="-I$top/ppinclude"

if [ -f $pathcfg ]; then
	x=`awk '/^[^#]/ { printf "-I%s ", $1 }' $pathcfg`
	stdinc=`eval echo \"$x\"`" $stdinc"
fi

#	create dependens

mkdep ()
{
	tee $tmp.1 | $ppfilter
	$pp2dep -lx $tmp $of < $tmp.1
	rm -f $tmp.1
}

#	plain filter, reduce empty lines

plain ()
{
	awk '
BEGIN { flag = 2; }
/^#/ { next }
/^ *$/ { if (flag == 0) flag = 1; next }
{
	if	(flag == 1) printf("\n");

	print
	flag = 0;
}
'
}

filter=mkdep

# parse command args

while getopts npgGc:d:I:D:U: opt
do
	case $opt in
	n)	filter="cat";;
	p)	filter="plain";;
	g)	CC="gcc";;
	G)	CC="gcc -traditional";;
	c)
		if	test A$OPTARG != A-
		then	cfg="$OPTARG"
		else	defcfg=""
		fi

		;;
	d)
		filter="$pp2dep -lx $tmp"
		mkdep=0;

		if	test A$OPTARG != A-
		then	filter="$filter $OPTARG"
		fi
		;;
	I)	
		if	test A$OPTARG = A-
		then	stdinc=""
		else	flags="$flags -I$OPTARG"
		fi
		;;

	D)	flags="$flags -D$OPTARG";;
	U)	flags="$flags -U$OPTARG";;
	\?)	usage; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

if [ $# -gt 2 ]; then
	usage
	exit 1
fi

test $# -ge 2 -a x$2 != x- && of=$2
test $# -ge 1 -a x$1 != x- && if=$1

test ! -f $if && { printf "$fmt_nodef" "$if"; exit 1; }

#	Test auf Bedarf einer Konfigurationsdatei

if	grep '^#include' $if >/dev/null; then
	:
elif	[ A$cfg = A ]; then
	cfg=$defcfg
	printf "$fmt_hdr" "$cfg"
fi

#	Verarbeiten

#echo $id $bootstrap
#echo $id "$@"
#echo if=$if of=$of

cat > $tmp <<!
#define	EFEUROOT	$top
#define	_BOOTSTRAP	$id $bootstrap
#define	IMAKEFILE	$if
#define	MakeFile	$of

!

if
	test ! A$cfg = A
then
	echo "#include <$cfg>" >> $tmp
fi

echo "#include \"$if\"" >> $tmp

$CC -I. $flags $stdinc -E $tmp | $filter > $of
rm -f $tmp

#	Falls das generierte Makefeile eine depend-Regel enthält,
#	wird diese aktiviert

if	grep '^depend[ ]*:' $of >/dev/null; then
	make -f $of depend
fi
