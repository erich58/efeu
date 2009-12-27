#!/bin/sh
#	Systemspezifische Parameter für Headerdateien generieren
#	(c) 1999 Erich Frühstück
#	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

tmp=/tmp/efeu$$
trap "rm -f $tmp*" 0
trap "exit 1" 1 2 3

CC=efeucc
flags=""
verbose=0
incl=""

#	Programmsyntax ausgeben

usage ()
{
	cat <<!
Aufruf: $0 [-hv] [-c name] [-f flgs] cmd arg(s)
!
	if	test $1 -eq 0
	then	exit 1
	fi

	cat <<!

	-h	Dieser Text
	-v 	Fehlermeldungen ausgeben
	-c name	C-Kompiler zum Testen
	-f flgs	Flags für C-Kompiler

	typedef <type> <decl>
		Falls der Type <type> nicht deklariert ist, wird
		er mit <decl> <type> definiert.
	func <tname> <expr>
		Falls der Aufruf <expr> erfolgreich ist, wird der
		Makro <tname> mit 1, ansonsten mit 0 definiert.
	byteorder
		Je nach Byteordnung wird der Makro <REVBYTEORDER>
		auf 1 oder 0 gesetzt.
	proto <proto>
		Testet die Kompatiblität des Prototypes <proto>.
!
	exit 0
}

#	Optionen abfragen

while getopts "hvc:f:i:" opt
do
	case $opt in
	h)	usage 1;;
	v)	verbose=1;;
	c)	CC="$OPTARG";;
	f)	flags="$OPTARG";;
	i)	incl="$incl
#include <$OPTARG>";;
	\?)	usage 0;;
	esac
done

shift `expr $OPTIND - 1`

if	[ $# -lt 1 ]
then	usage 0
fi

decl=""
body=""
success=":"
failure=":"
type=$1
shift

#	Test bestimmen

case $type in
typedef)
	name=$1; shift
	failure="echo typedef $* $name;"
	body="$name x;"
	;;
func)
	success="echo #define $1 1"
	failure="echo #define $1 0"
	body="$2;"
	;;
byteorder)
	success="$tmp"
	failure="echo #error Byteordung nicht identifiziert."
	body='int x = 1;
printf("#undef	REVBYTEORDER\n");
printf("#define	REVBYTEORDER %d\n", ((char *) &x)[0]);'
	;;
proto)
	success="echo $*;"
	failure="echo Prototype nicht kompatibel >&2"
	decl="$*;"
	;;
test)
	success="echo Erfolg"
	failure="echo Fehlschlag"
	body="$*;"
	;;
esac

#	Test durchführen

cat > $tmp.c <<!
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
$incl
$decl

int main (void)
{
$body
return 0;
}
!

if	$CC $flags -o $tmp $tmp.c 2> $tmp.log
then	$success
else	$failure
fi

if	[ $verbose -gt 0 ]; then
	cat $tmp.c >&2
	cat $tmp.log >&2
fi

rm -f $tmp.c $tmp.s $tmp.log
