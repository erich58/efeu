#	SGML-Dokument konvertieren
#	(c) 1999 Erich Frühstück
#	A-3423 St. Andrä/Wördern, Südtirolergasse 17-21/5

#	Die sgml_tools können nicht als pipe verwendet werden.
#	Daher erfolgt die Verarbeitung in einer eigenen Bibliothek

tmpdir=/var/tmp/edoc$$
name=$tmpdir/doc

trap "rm -rf $tmpdir" 0
trap "exit 1" 1 2 3

#	Programmsyntax ausgeben

OPTDEF="hlt:"

usage ()
{
	cat <<!
Aufruf: $0 [-hl] [-t type] name[.sgml]

	-h		Dieser Text
	-l		Typen auflisten
	-t type		Ausgabetype
	name		Definitionsfile
!
}

list ()
{
	cat <<!
man	Handbuch-Source für nroff/troff
nroff	nroff-Formatierung über Handbuchsourcen
txt	Textausgabe mit Attributen
filter	Textausgabe ohne Attribute
html	HTML-Dokument
latex	LaTeX-Dokument
ps	PostScript-Dokument
!
}

type=txt
flags="-p a4 -c latin --language=de"

#	Optionen abfragen

while getopts $OPTDEF opt
do
	case $opt in
	t)	type="$OPTARG";;
	l)	list; exit 0;;
	h)	usage; exit 0;; 
	\?)	usage | sed -e '/^$/q'; exit 1;;
	esac
done

shift `expr $OPTIND - 1`
test $# -lt 1 -o $# -gt 2 && { usage | sed -e '/^$/q'; exit 1; }

#	Basisfile generieren

mkdir $tmpdir || exit 1

if [ A$1 = A- ]; then
	cat > $tmpdir/doc.sgml
elif [ -f $1 ]; then
	cp $1 $tmpdir/doc.sgml
elif [ -f $1.sgml ]; then
	cp $1.sgml $tmpdir/doc.sgml
else
	echo "$0: Eingabefile $1 ist nicht lesbar" >&2 
fi

unset LANG
#echo "Type=$type" >&2

case $type in
man)
	( cd $tmpdir; sgml2txt $flags -m doc >&2 )
	cat $tmpdir/doc.man
	;;
nroff)
	( cd $tmpdir; sgml2txt $flags -m doc >&2 )
	nroff -Tlatin1 -man $tmpdir/doc.man
	;;
latex)
	( cd $tmpdir; sgml2latex $flags --output=tex doc >&2 )
	cat $tmpdir/doc.tex
	;;
ps)
	( cd $tmpdir; sgml2latex $flags --output=ps doc >&2 )
	cat $tmpdir/doc.ps
	;;
filter)
	( cd $tmpdir; sgml2txt $flags -f doc >&2 )
	cat $tmpdir/doc.txt
	;;
html)
	( cd $tmpdir; sgml2html $flags -s 0 doc >&2 )
	cat $tmpdir/doc.html
	;;
cgi)
	echo "Content-type:text/plain"
	echo
	( cd $tmpdir; sgml2html $flags -s 0 doc >&2 )
	cat $tmpdir/doc.html
	;;
*)
	( cd $tmpdir; sgml2txt $flags doc >&2 )
	cat $tmpdir/doc.txt
	;;
esac
