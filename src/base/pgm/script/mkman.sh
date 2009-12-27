#!/bin/sh

tmp=/usr/tmp/ml$$

cleanup()
{
#	echo "Aufräumen"
	rm -f $tmp*
}

trap "cleanup" 0
trap "exit 1" 1 2 3

#	Parameter initialisieren

SEC=3

usage()
{
	cat << !
Aufruf: $0 [-h] [-f file] [-s sec] name dir(s)

	-h	Hilfetext ausgeben
	-f	Liste aus Datei generieren
	-s sec	Handbuchabschnitt, Vorgabe $SEC	
	name	Gruppenname
	dir(s)	Bibliothek für Sourcen
!
}

#	Aufarbeiten der Argumentliste

if
	set -- `getopt hs:f: "$@" 2>&1`
	test $? -ne 0
then
	shift
	echo $0: $*
	exit 1
fi

cat /dev/null > $tmp.def

while
	test $# -gt 0
do
	case $1 in
	-h)	usage; exit 0;;
	-f)	shift; cat $1 > $tmp.def; shift;;
	-s)	shift; SEC=$1; shift;;
	--)	shift; break;;
	*)	echo "$0: Illegaler Parameter $1"; exit 1;;
	esac
done

if
	test $# -lt 2
then
	usage
	exit 1
fi

NAME=$1
shift

makefile=$NAME.make
listfile=$NAME.tex

#	Liste der lokalen Files generieren

cat /dev/null > $tmp.1

for x in `find $* -type d -print`
do
	if
		test -f $x/LOCALFILES
	then
		sed	-e '/^[ 	]*#/d'	\
			-e '/^[ 	]*$/d'	\
			-e "s|^[ 	]*|$x/|" \
			$x/LOCALFILES >> $tmp.1
	fi
done

if
	test -s $tmp.1
then
	sed -e 's/[./]/\\&/g' -e 's|^.*$|/^&/d|' $tmp.1 > $tmp.2
	find $* -type f -name "*.$SEC*" -print | sed -f $tmp.2 > $tmp.1
else
	find $* -type f -name "*.$SEC*" -print > $tmp.1
fi

if
	test `wc -l < $tmp.def` -gt 0
then
	fgrep -f $tmp.def $tmp.1 > $tmp.2
	mv $tmp.2 $tmp.1
fi

#	Filenamen zerlegen 

sed -e 's|[^/]*$| &|' -e 's|/ | |' -e 's/[^\.]*$/ &/' -e 's/\. / /' $tmp.1 > $tmp.lst

#	Test auf Mehrfachdefinition

awk '{ printf("%s.tex\n", $2); }' $tmp.lst | sort > $tmp.1
sort -u $tmp.1 | cmp -s - $tmp.1

if
	test $? -ne 0
then
	sort -u $tmp.1 | comm -13 - $tmp.1 > $tmp.2
	awk '{ printf("%s.tex durch %s/%s.%s\n", $2, $1, $2, $3) }' $tmp.lst | sort > $tmp.3
	echo "Die folgenden Handbucheinträge sind mehrfach definiert:\n"
	join $tmp.2 $tmp.3
	exit 1
fi

if
	test ! -d $NAME
then
	if
		mkdir $NAME
	then
		echo "Bibliothek $NAME generiert"
	else
		echo "Bibliothek $NAME kann nicht generiert werden"
		exit 1
	fi
fi

sort -f $tmp.1 | sed -e "s|^|\\\\input $NAME/|" > $listfile

#	Makefile generieren

awk '
BEGIN {
	printf("DEP= /efeu/bin/manpage /efeu/lib/efeu/app-defaults/manpage.cfg\n\n");
}
{
	target = sprintf("%s.tex", $2);
	src = sprintf("%s/%s.%s", $1, $2, $3);
	printf("all:: %s\n\n", target);
	printf("clean::\n\trm -f %s\n\n", target);
	printf("%s: %s $(DEP)\n\tmanpage -tex %s %s\n\n", target, src, src, target);
}' $tmp.lst > $NAME/Makefile

cd $NAME
make
