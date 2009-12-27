#	Begriffsliste generieren

bgdir=/efeu/doc/Begriffe
file=
cmd=

#	Aufrufsyntax

usage ()
{
	cat <<!
Aufruf: $0 [-h] [-a] [-d dir] [-s sub] [-f file] [Begriff(e)]

	-h	Dieser Text
	-a	Alle Begriffe auflisten
	-d dir	Begriffe in Bibliothek dir suchen
	-s sub	Begriffe in Unterbibliothek sub suchen
	-f file	Begriffsliste aus Datei zusammenstellen

Das Kommando generiert zu einer Liste von Begriffen die zugehörige
Liste der Datenfiles.
!
}

while getopts d:s:f:nha opt
do
	case $opt in
	a)	cmd="\\listall";;
	d)	bgdir=$OPTARG;;
	s)	bgdir="$bgdir/$OPTARG";;
	f)	file="$OPTARG";;
	h)	usage; exit 0;;
	\?)	usage | sed -e '/^$/q'; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

(
	find $bgdir -type f -print | sed -e 's/^/\\include /'
	echo $cmd
	cat /dev/null $file | sed -e 's/^/\\entry /'

	for x in $*
	do
		echo "\\entry" $x
	done
) | efeudoc -C bglist.dmac -o /dev/null -
