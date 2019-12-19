#!/bin/sh
# :*:Update symlinks
# :de:Symlinks aktualisieren

#	message formats

msg1="usage: $0 [--help] base\n"

case ${LANG:=en} in
de*)
	msg1="Aufruf: $0 [--help] base\n"
	;;
esac
 
# $pconfig
# Version=":VN:"
# n |
#	:*:dry run
#	:de:Trockenlauf
# s:suffix |
#	:*:suffic of filename
#	:de:Dateiendung
# :base |
#	:*:base name
#	:de:Basisname

usage ()
{
	efeuman -- $0 $1 || printf "$msg1"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

cmd="ln -vfs"
suffix=""

while getopts "ns:" opt
do
	case $opt in
	n)	cmd="echo $cmd";;
	s)	suffix="$OPTARG";;
	\?)	usage -?; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

dir=`dirname $1`
base=`basename $1`

cd $dir || exit 1

while read name
do
	$cmd $base $name$suffix
done
