cpio_flags="-pdmv"


case $1 in
	-l)	shift
		cpio_flags="-pdmvl"
		;;
esac

if
	test $# -lt 2
then
	echo "\
Aufruf: $0 [-l] Alte_Bibliothek Neue_Bibliothek [find-Optionen]

	-l	Wenn möglich, Dateien linken statt kopieren
"
	exit 1
elif
	test ! -d $1
then
	echo "Bibliothek $1 existiert nicht"
	exit 1
elif
	test ! -d $2
then
	echo "Bibliothek $2 existiert nicht"
	exit 1
else
	source=$1
	target=$2
	shift 2
	echo find $source "$@" -depth -print
	find $source "$@" -depth -print | cpio $cpio_flags $target
fi
