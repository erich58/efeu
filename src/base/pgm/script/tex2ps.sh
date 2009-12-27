#!/bin/sh
#	TeX-Formatierung
#	(c) 1993 Erich Fruehstueck
#	A-1090 Wien, Waehringer Strasse 64/6

name=TeX$$
texfile=$name.tex
dvifile=$name.dvi
psfile=$name.ps
count=1
#TEXINPUTS=.:`pwd`:/usr/lib/texmf/texmf/tex//
#export TEXINPUTS

usage ()
{
	echo "Aufruf: $0 [-h] [-q] [-n count] [-r res] src [ps]"
	exit 1
}

help ()
{
	cat <<!
Aufruf: $0 [-h] [-q] [-n count] [-r res] src [ps]

	-h	Dieser Text
	-q	Landscape
	-n	Zahl der Formatierungsläufe
	-r	Auflösung
	src	Definitionsfile
	ps	Ausgabefile

!
	exit 0
}

#	Aufarbeiten der Argumentliste

if
	set -- `getopt hn:qr: "$@" 2>&1`
	test $? -ne 0
then
	shift
	echo $0: $*
	exit 1
fi

#	Optionen abfragen

for i
do
	case $i in
	-h)	help;;
	-n)	count=$2; shift 2;;
	-q)	psflags="$psflags -t landscape"; shift;;
	-r)	psflags="$psflags -D$2"; shift 2;;
	--)	shift; break;;
	esac
done

if
	test $# -lt 1
then
	usage;
fi

#	Eingabefile generieren

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
	echo "$0: Datei \"$1\" nicht gefunden." >&2
fi


#	Formatieren mit tex oder latex

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

echo "Formatieren..." >&2

while [ $count -gt 0 ]
do
	$cmd $texfile < /dev/null >&2

	if [ -f $name.idx ]; then
		makeindex $name
	fi

	count=`expr $count - 1`
done

case `basename $0` in
	tex2ps)	echo "Konvertieren..." >&2
		dvips $psflags $dvifile -o $psfile
		outfile=$psfile
		;;
	*)	outfile=$dvifile
		;;
esac

#	Ausgabefile kopieren

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

#	Aufraeumen

rm -f $name.*
