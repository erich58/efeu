#!/bin/sh
# :*:run unit test with code coverage analysis
# :de:Start von Unit Tests mit Analyse der Testabdeckung

#	message formats

msg1="usage: $0 [--help] [-q] [-C dir] [-o out] name\n"

case ${LANG:=en} in
de*)
	msg1="Aufruf: $0 [--help] [-q] [-C dir] [-o out] name\n"
	;;
esac
 
# $pconfig
# Version=":VN:"
# q |
#	:*:do not show details
#	:de:Keine Detaildaten ausgeben
# o:out |
#	:*:output file for detail data
#	:de:Ausgabefile für Detaildaten
# c:base |
#	:*:directory with basic evaluation
#	:de:Verzeichnis mit Basisauswertung
# :name |
#	:*:build/analyse directory or single command
#	:de:Generierungs-/Analyseverzeichnis oder Einzelkommando

usage ()
{
	efeuman -- $0 $1 || printf "$msg1"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

out="test.out"
gcov="gcov"
base=""

while getopts "qo:c:t:" opt
do
	case $opt in
	q)	gcov="gcov -n"; out="/dev/null";;
	o)	out=$OPTARG;;
	c)	base=$OPTARG;;
	\?)	usage -?; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

case "$#" in
0)	usage -?; exit 0;;
1)	;;
*)	usage -?; exit 1;;
esac

find_tst()
{
	find $1 -name '*.tst' | sort
}

if [ -d $1 ]; then
	dir=$1
	tlist="find_tst"
else
	dir=`dirname $1`
	tlist="echo";
fi

if	[ "$base" ]; then

	grep -l . "$base/gcov.out" >/dev/null || exit 1;

	find $dir -name '*.c' ! -name '*.tst.c' -printf '%f\n' | while read name
	do
		grep "$name" $base/gcov.out
	done
	exit 0;
fi

# cleanup coverage data

find $dir -name '*.gcda' -exec rm '{}' \;
find $dir -name '*.c.gcov' -exec rm '{}' \;

# run unit tests

echo "#unit tests"

redir=""

if [ $out ]; then
	[ -f $out ] && rm -f $out
	redir="2>> $out"
fi

$tlist $1 | while read tst
do
	eval $tst $redir
done | awk '
BEGIN {
	fmt = "%-23s\t%3d\t%3d\t%3d\t%s\n";
	t = 0; g = 0; b = 0; s = "SUCCESS"
}
/FAILURE/ {
	s = "FAILURE"
}
{
	t += $2; g += $3; b += $4;
	printf(fmt, $1, $2, $3, $4, $5);
}
END {
	printf(fmt, "TOTAL", t, g, b, s);
}
'

# analyse coverage

export LANG=C
echo "#code coverage"

find $dir -name '*.gcov.o' | sort | while read obj
do
	name=`basename $obj .gcov.o`

	if	[ -f $dir/$name.gcov.gcda ]; then
		$gcov -o $dir $obj | sed -e "s/'//g" |
			awk -v src=$dir/$name.c.gcov '
/:0.00% of/ { printf("#%-22s\tnot used\n", src); next; }
/^Lines/ { printf("%-23s\t%s\n", src, substr($0, index($a, ":") + 1)); }
'
		[ -f $name.c.gcov ] && mv $name.c.gcov $dir
	else
		printf '#%-22s\tnot run\n' $name.c
	fi
done | tee gcov.out | awk '
BEGIN { run = 0; c = 0.; n = 0; unused = 0; other = 0; }
/not used/ { unused++; next; }
/not run/ { other++; next; }
{
	c += $2 * $4;
	n += $4
	run++;
}
END {
	printf("Coverage: %.2f%% of %d lines in %d files\n",
		n ? c / n : 0, n, run);

	if	(unused)
		printf("Not used: %d files\n", unused);
	if	(other)
		printf("Missing:  %d files\n", other);
}
'

exit 0
