#!/bin/sh
#	Makefile aus Imakefile generieren
#	(c) 1999 Erich Frühstück
#	A-3423 St. Andrä/Wördern, Südtirolergasse 17-21/5

CC="@CC@"
top="@TOP@"
cfg="mkmf.cfg"

ppfilter="$top/bin/ppfilter"
pp2dep="$top/bin/pp2dep"
stdinc="-I$HOME/config -I$top/ppinclude"
flags="-D_EFEU_TOP=$top"
tmp=/usr/tmp/pp$$.c
if=Imakefile
of=Makefile
id=$0
bootstrap="$@"

#	Programmsyntax ausgeben

usage ()
{
	cat <<!
Aufruf: $0 [-hnugG] [-c cfg] [-d target] [-I dir] [-U name] [-D name[=val]] file
!
	if	test $1 -eq 0
	then	exit 1
	fi

	cat <<!

	-h		Dieser Text
	-n		Keinen Ausgabefilter
	-p		Plain-Filter
	-g		GNU - Preprozessor (ANSI) verwenden
	-G		GNU - Preprozessor (traditionell) verwenden
	-c name		Konfigurationsdatei, Vorgabe $cfg
	-d target	Abhängigkeitsregel für target generieren
	-I dir		Suchpfad um dir erweitern
	-U name		Makro name löschen
	-D name=[val]	Makro name definieren
	src		Definitionsfile

!
	exit 0
}

#	Makefile mit Abhängigkeiten

mkdep ()
{
	tee $tmp.1 | $ppfilter
	$pp2dep -lx $tmp $of < $tmp.1
	rm -f $tmp.1
}

#	Leerzeilenfilter

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

#	Optionen abfragen

while getopts hnpgGc:d:I:D:U: opt
do
	case $opt in
	h)	usage 1;;
	n)	filter="cat";;
	p)	filter="plain";;
	g)	CC="gcc";;
	G)	CC="gcc -traditional";;
	c)
		if	test A$OPTARG != A-
		then	cfg="$OPTARG"
		else	cfg=""
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
	esac
done

shift `expr $OPTIND - 1`

test $# -gt 2 && usage 0
test $# -ge 2 -a x$2 != x- && of=$2
test $# -ge 1 -a x$1 != x- && if=$1

test ! -f $if && { echo "$id: Definitionsfile $if nicht lesbar"; exit 1; }

#	Verarbeiten

#echo $id $bootstrap
#echo $id "$@"
#echo if=$if of=$of

cat > $tmp <<!
@!	Nicht editieren, Datei wurde aus $if generiert

BOOTSTRAP =	$id $bootstrap
EFEUTOP =	$top
CFGSTAMP =	$top/`uname -s`_`uname -m`

#define	IMAKEFILE	$if
#define	MAKEFILE	$of
#define	INCLUDE_IMAKEFILE "$if"

!

if
	test A$cfg = A
then
	echo "#include INCLUDE_IMAKEFILE" >> $tmp
else
	echo "#include <$cfg>" >> $tmp
fi

$CC -I. $flags $stdinc -E $tmp | $filter > $of

rm -f $tmp
