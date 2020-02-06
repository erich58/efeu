#!/bin/sh
# :*:run unit test with code coverage analysis
# :de:Start von Unit Tests mit Analyse der Testabdeckung

#	message formats

msg1="usage: $0 [--help] [-q] [-b dir] [-o out] name\n"

case ${LANG:=en} in
de*)
	msg1="Aufruf: $0 [--help] [-q] [-b dir] [-o out] name\n"
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
# b:build |
#	:*:name of subdirectory with build files
#	:de:Unterverzeichnis mit generierten Dateien
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
build=".build"

while getopts "qo:b:" opt
do
	case $opt in
	q)	gcov="gcov -n"; out="/dev/null";;
	o)	out=$OPTARG;;
	b)	build=$OPTARG;;
	\?)	usage -?; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

case "$#" in
0)	usage -?; exit 0;;
1)	;;
*)	usage -?; exit 1;;
esac

error()
{
	printf '%s\n' "$*"
	exit 1;
}

find_tst()
{
	find $1 -name '*.tst.c' | sort | while read path
	do
		name=`basename $path .c`
		[ -f $2/$name ] && echo $2/$name
	done
}

find_gcda()
{
	find $1 -name '*.c' ! -name '*.tst.c' | sort | while read path
	do
		name=`basename $path .c`
		gcda=$2/$name.gcov.gcda
		[ -f $gcda ] && echo $gcda
	done
}

if [ -d $1/$build ]; then
	dir=.
	top=$1
	src=$1/$build
elif [ -d $1 ]; then
	dir=`basename $1`
	top=`dirname $1`
	src=$dir
elif [ -f $1 ]; then
	dir=`dirname $1`
	top=`dirname $dir`
	src=`basename $dir`/`basename $1`
else
	error "$0: Testliste $1 nicht definiert."
fi

cd $top || exit 1

if [ ! -d $build ]; then
	error "$0: Build-Verzeichnis nicht definiert."
fi

[ -z "$tlist" ] && tlist=`find $src -name '*.tst' | sort`
[ -z "$tlist" ] && tlist=`find_tst $src $build`
[ -z "$tlist" ] && error "$0: Keine Tests gefunden"

# cleanup coverage data

find $build -name '*.gcda' -exec rm '{}' \;
find $build -name '*.gcov' -exec rm '{}' \;

# run unit tests

echo "#unit tests"

redir=""

if [ $out ]; then
	[ -f $out ] && rm -f $out
	redir="2>> $out"
fi

for tst in $tlist
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

gcda=`find_gcda $src $build`

if	[ -z "$gcda" ]; then
	gcda=`find $build -name '*.gcda' | sort`
	verbose=0
else
	verbose=1
fi

$gcov -o $build $gcda | sed -e "s/'//g" | awk '
/File/ { src=$2; }
NF == 0 { src = "TOTAL"; }
/:0.00% of/ { printf("#%-22s\tnot used\n", src); next; }
/^Lines/ { printf("%-23s\t%s\n", src, substr($0, index($a, ":") + 1)); }
' | tee gcov.out | awk -v verbose=$verbose '
BEGIN { run = 0; c = 0.; n = 0; unused = 0; other = 0; }
/TOTAL/ { print; next; }
/not used/ { unused++; next; }
/not run/ { other++; next; }
{
	c += $2 * $4;
	n += $4
	run++;
}
verbose { print; }
END {
	printf("Coverage: %.2f%% of %d lines in %d files\n",
		n ? c / n : 0, n, run);

	if	(unused)
		printf("Not used: %d files\n", unused);
	if	(other)
		printf("Missing:  %d files\n", other);
}
'

find . -maxdepth 1 -name '*.gcov' -exec mv -f '{}' $build \;
exit 0
