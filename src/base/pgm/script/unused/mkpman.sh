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

usage()
{
	cat << !
Aufruf: $0 [-h] [-f file] name pgm(s)

	-h	Hilfetext ausgeben
	-f file	Datei mit Programmen
	name	Gruppenname
	pgm(s)	Programme
!
}

#	Aufarbeiten der Argumentliste

if
	set -- `getopt hf: "$@" 2>&1`
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
	-f)	shift; cat $1 >> $tmp.def; shift;;
	--)	shift; break;;
	*)	echo "$0: Illegaler Parameter $1"; exit 1;;
	esac
done

if
	test $# -lt 1
then
	usage
	exit 1
fi

NAME=$1
shift
makefile=$NAME.make
listfile=$NAME.tex

for i in $*
do
	echo $i >> $tmp.def
done

#	Programmname und TeX-Name generieren

awk -F"/" '{printf("%s.tex\t%s\n", $NF, $0)}' $tmp.def > $tmp.lst

#	Test auf Mehrfachdefinition

awk '{ printf("%s\n", $1); }' $tmp.lst | sort > $tmp.1
sort -u $tmp.1 | cmp -s - $tmp.1

if
	test $? -ne 0
then
	sort -u $tmp.1 | comm -13 - $tmp.1 | uniq > $tmp.2
	awk '{ printf("%s durch %s\n", $1, $2) }' $tmp.lst | sort > $tmp.3
	echo "Die folgenden Handbucheinträge sind mehrfach definiert:\n"
	join $tmp.2 $tmp.3
	exit 1
fi

if
	test `wc -l < $tmp.lst` -eq 0
then
	echo "Kein Eintrag definiert"
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

sed -e "s|^|\\\\input $NAME/|" $tmp.1 > $listfile

#	Makefile generieren

awk '
BEGIN {
	printf("DEP=	/usr/local/lib/efeu/de_DE/app-defaults/manpage.cfg\n\n");
}
{
	printf("all:: %s\n\n", $1);
	printf("clean::\n\trm -f %s\n\n", $1);
	printf("%s: $(DEP)\n\t%s -help=tex > $@\n\n", $1, $2);
}' $tmp.lst > $NAME/Makefile

cd $NAME
make
