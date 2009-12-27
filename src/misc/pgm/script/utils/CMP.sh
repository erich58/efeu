pgm=cmp
verbose=0

case $1 in
	-c)	shift
		pgm="diff -c"
		;;
	-d)	shift
		pgm="diff"
		;;
	-l)	shift
		pgm="cmp -l"
		verbose=1
		;;
	-v)	shift
		verbose=1
		;;
esac

if
	test $# -lt 2
then
	echo "\
Aufruf: $0 [-lcdv] dir1 dir2 [find-Optionen]

	-l	Ausgabe aller abweichenden Bytes
	-d	Vergleich mit diff statt mit cmp
	-c	Kontextvergleich mit diff statt mit cmp
	-v	Protokoll der durchgeführten Vergleiche

Das Kommando vergleicht Dateien der Bibliothek dir1 mit denen
in Bibliothek dir2.
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

	( cd $source; find . -type f "$@" -print ) | (
	while
		read x
	do
		if
			test $verbose -gt 0
		then
			echo Vergleich: $source/$x $target/$x
		fi

		$pgm $source/$x $target/$x
	done
	)	
fi
