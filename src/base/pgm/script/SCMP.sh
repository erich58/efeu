pgm=cmp
verbose=0;

case $1 in
	-c)	shift; pgm="diff -c";;
	-d)	shift; pgm="diff";;
	-z)	shift; pgm="z$pgm";;
	-v)	shift; verbose=1;;
esac

if
	test $# -ne 1
then
	echo "\
Aufruf: $0 [-cdv] Sicherungsbibliothek

	-d	Vergleich mit diff statt mit cmp
	-c	Kontextvergleich mit diff statt mit cmp
	-z	Vergleich mit zcmp/zdiff
	-v	Protokoll der durchgeführten Vergleiche
"
	exit 1
elif
	test ! -d $1
then
	echo "Bibliothek $1 existiert nicht"
	exit 1
else
	find $1 -type f -print | (
	while
		read x
	do
		y=`expr $x : $1'/\(.*\)'`

		if
			test $verbose -gt 0
		then
			echo Vergleich: $x $y
		fi

		$pgm $x $y
	done)	
fi
