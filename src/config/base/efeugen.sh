#!/bin/sh
#	Standardziele generieren

top=${EFEUTOP:=@TOP@}
gendir=${EFEUGEN:=$top/build}
flags=

#	Aufrufsyntax

usage ()
{
	cat <<!
Aufruf: $0 [-hn] [-d dir] [-s sub] [Ziele(e)]

	-h	Dieser Text
	-n	Regeln nur anzeigen
	-d dir	make in Bibliothek dir aufrufen
	-s sub	make in Unterbibliothek sub.d aufrufen

Das Kommando wechselt in die Bibliothek $gendir und ruft make
mit den angegebenen Argumenten auf. Die Standardbibliothek
für efeugen ist @TOP@/build und kann mit der Umgebungsvariablen 
EFEUGEN umgesetzt werden. Die Option "-s" kann mehrfach verwendet
werden. Eine Unterbliothek der Form "a.b" wird in "a.d/b.d" konvertiert
("-s a.b" ist damit äquivalent zu "-s a -s b").
!
}

while getopts d:s:nh opt
do
	case $opt in
	d)	gendir=$OPTARG;;
	s)	gendir="$gendir/"`echo $OPTARG | sed -e 's/\./\.d\//g'`".d";;
	n)	flags="$flags -n";;
	h)	usage; exit 0;;
	\?)	usage | sed -e '/^$/q'; exit 1;;
	esac
done

shift `expr $OPTIND - 1`
cd $gendir || exit 1

if
	test -f Makefile -o -f makefile
then
	make $flags "$@"
else
	echo "$0:" Kein Makefile in `pwd` vorhanden.
fi
