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
# r |
#	:*:do not show files not running
#	:*:Ungenutzte Dateien nicht ausgeben
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
not_run="1"

while getopts "qro:b:" opt
do
	case $opt in
	q)	gcov="gcov -n"; out="/dev/null";;
	r)	not_run="0";;
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
	find $1 -name '*.tst' -perm -u+x
	find $1 -name '*.tst.c' | while read path
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

used=0

if [ -d $1/$build ]; then
	dir=.
	top=$1
	src=.
elif [ -d $1 ]; then
	dir=`basename $1`
	top=`dirname $1`
	src=$dir
elif [ -f $1 ]; then
	dir=`dirname $1`
	top=`dirname $dir`
	src=`basename $dir`/`basename $1`
	used=1
else
	error "$0: Testliste $1 nicht definiert."
fi

cd $top || exit 1

if [ ! -d $build ]; then
	error "$0: Build-Verzeichnis nicht definiert."
fi

tlist=`find_tst $src ./$build | sort -u`
[ -z "$tlist" ] && error "$0: Keine Tests gefunden"

# cleanup coverage data

find $build -name '*.gcda' -exec rm '{}' \;
find $build -name '*.[ch].gcov' -exec rm '{}' \;

# run unit tests

echo "#unit tests"

redir=""

if [ $out ]; then
	[ -f $out ] && rm -f $out
	redir="2>> $out"
fi

red=`tput setaf 1`
green=`tput setaf 2`
off=`tput sgr0`
SUCCESS="${green}SUCCESS{$off}"

for tst in $tlist
do
	eval $tst $redir
	stat=$?
	[ $stat -gt 128 ] && printf '%s 0 0 1 SIG-%02d\n' \
		`basename $tst` `expr $stat - 128`
done | awk -v good="$green" -v bad="$red" -v off="$off" '
BEGIN {
	fmt = "%-23s\t%5d\t%5d\t%5d\t%s%s%s\n";
	t = 0; g = 0; b = 0;
}
{
	t += $2; g += $3; b += $4;
	printf(fmt, $1, $2, $3, $4, $4 ? bad : good, $5, off);
}
END {
	if	(b)
		printf(fmt, "TOTAL", t, g, b, bad, "FAILURE", off);
	else
		printf(fmt, "TOTAL", t, g, b, good, "SUCCESS", off);
}
'

# analyse coverage

export LANG=C
echo ""
echo "#code coverage"

gcda=""

[ "$src" != "." ] && gcda=`find_gcda $src $build`

if	[ -z "$gcda" ]; then
	gcda=`find $build -name '*.gcda' | sort`
	verbose=$used
else
	verbose=2
fi

$gcov -o $build $gcda | sed -e "s/'//g" | awk '
/File/ { src=$2; }
NF == 0 { src = "~TOTAL"; }
/^Lines/ { printf("%s\t%s\n", src, substr($0, index($a, ":") + 1)); }
' | sed -e '/.tst.[ch]	/d' > gcov.tmp

if [ $not_run -eq 1 ]; then
	find $dir -name '*.c' ! -name '*.tst.c' | while read path
	do
		name=`basename $path .c`
		[ -f $name.c.gcov ] || printf '%s\tnot run\n' $path |
			sed -e 's|^\./||' >> gcov.tmp
	done
fi

find . -maxdepth 1 -name '*.[ch].gcov' -exec mv -f '{}' $build \;

sort gcov.tmp | awk -v not_run=$not_run '
BEGIN { n = 0; cn = 0; kn = 0; }
$2 ~ /not/ { printf("%-23s\t%s %s\n", $1, $2, $3); next; }
$1 ~ /TOTAL/ { next; }
{
	k = 0.01 * $2 * $4;

	if	(k > 0. || not_run == 1)
		printf("%-23s\t%.0f\t%.0f\t%s\n", $1, k, $4, $2);

	n++;
	kn += k
	cn += $4
	next;
}
END {
	printf("TOTAL %3d %-13s\t%.0f\t%.0f\t%.2f%%\n",
		n, "files", kn, cn, cn ? (100. * kn / cn) : 0.);
}
' | tee gcov.out | awk -v verbose=$verbose '
BEGIN { run = 0; c = 0.; n = 0; k = 0; unused = 0; other = 0; }
/^TOTAL/ { next; }
verbose {
	if	($2 > 0 || verbose > 1)
		print;
}
/not used/ { unused++; next; }
/not run/ { other++; next; }
{
	n += $3;
	k += $2;
	c += $3 * $4;

	if	($2 == 0)
	{
		unused++;
		next;
	}

	run++;
}
END {
	if	(verbose)
		printf("\n");

	printf("Coverage: %.2f%% %d of %d lines in %d files\n",
		n ? c / n : 0, k, n, run);

	if	(unused)
		printf("Not used: %d files\n", unused);
	if	(other)
		printf("Missing:  %d files\n", other);
}
'

rm gcov.tmp
exit 0
