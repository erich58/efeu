#	Alle Programme generieren

gen=$HOME/libgen
list="efm efx md"
pgm=$HOME/pgm

#	Aufarbeiten der Argumentliste

usage ()
{
	cat <<!
Aufruf: $0 [-h0xmMst:] [Programm(e)]

	-0	Nur efm-Bibliothek
	-x	Nur efx-Bibliothek
	-M	Nur Math-Bibliothek
	-m	Nur md-Bibliothek
	-s	Nur Synthesis-Bibliothek
	-p	Nur Pix-Bibliothek
	-w	Nur efwin-Bibliothek
	-a	Alle Bibliotheken
	-t	Generierungsziel
!
}

if
	set -- `getopt h0xMmspwa "$@" 2>&1`
	test $? -ne 0
then
	echo $*
	usage
	exit 1
fi

for i
do
	case $i in
	-h)	shift; usage; exit 0;;
	-0)	shift; list="efm";;
	-x)	shift; list="efx";;
	-M)	shift; list="efmath";;
	-m)	shift; list="md";;
	-s)	shift; list="synt";;
	-p)	shift; list="pix";;
	-w)	shift; list="efwin";;
	-a)	shift; list="efm efx efmath md pix efwin";;
	--)	shift; break;;
	esac
done


for i in $list
do
	(cd $gen; make $i)
done

for i in $*
do
	(cd $pgm/$i; make)
done
exit 0
