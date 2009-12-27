if
	test $# -lt 2
then
	echo "Aufruf: $0 dir [grep-Optionen] pattern"
else
	dir="$1";
	shift;
	exec find $dir -type f -print | xargs grep "$@"
fi
