
if
	test $# -ne 1
then
	echo "Aufruf $0 Filename"
	exit 1
elif
	test ! -r $1
then
	echo "$0: File \"$1\" nicht lesbar"
	exit 1
elif
	test ! -w $1
then
	echo "$0: File \"$1\" nicht beschreibbar"
	exit 1
fi

echo "
Das Programm formt die Binärdatei \"$1\" in eine temporäre
Datei um, die mit dem vi - Editor editiert werden kann.
"
if
	echo "Fortfahren ? j\b\c" 1>&2
	read x
	test A$x != An
then
	tmp=/usr/tmp/xvi$$
	mkdump $1 $tmp
	vi $tmp

	if
		echo "Änderungen sichern ? j\b\c" 1>&2
		read x
		test A$x != An
	then
		undump $tmp $1
	fi

	rm $tmp
fi
