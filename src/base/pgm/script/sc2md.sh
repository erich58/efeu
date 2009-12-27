flags=

if
	test X$1 = X-r
then
	flags="$flags -r";
	shift;
fi

if
	test $# -ne 2
then
	echo "Aufruf $0 ein aus"
	exit 1
fi

tmp=${TMPDIR:-/tmp}/sc2md$$

sc $1 >/dev/null <<!
T$tmp
q
!

mdread $flags $tmp $2
rm -f $tmp
