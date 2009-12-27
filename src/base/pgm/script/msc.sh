
if
	test $# -lt 1
then
	echo "Aufruf $0 Filename"
	exit 1
fi

tmp=/usr/tmp/mvi$$
bak="/usr/tmp/#mvi$$~"
name=$1
shift

if
	test ! -r $name
then
	echo "$0: File \"$name\" nicht lesbar"
	exit 1
elif
	test ! -w $name
then
	echo "$0: File \"$name\" nicht beschreibbar"
	exit 1
fi

echo "
Das Programm formt die Datenamtrix \"$name\" mit dem Kommando
in eine tempor�re Datei um, die mit dem vi - Editor editiert werden kann.
"
if
	echo mdprint -sc $name $tmp "$@"
	echo -e "Fortfahren ? j\b\c" 1>&2
	read x
	test A$x != An
then
	mdprint -sc $name $tmp "$@"
	sc $tmp

	if
		echo -e "�nderungen sichern ? j\b\c" 1>&2
		read x
		test A$x != An
	then
		sc2md $tmp $name
	fi

	rm -f "$tmp" "$bak"
fi
