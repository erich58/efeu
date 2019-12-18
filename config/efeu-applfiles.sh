#!/bin/sh
# :*:Update application files
# :de:Aktualisieren von Anwendungsdateien

#	message formats

msg1="usage: $0 [--help] dir [all|clean]\n"

case ${LANG:=en} in
de*)
	msg1="Aufruf: $0 [--help] dir [all|clean]\n"
	;;
esac
 
# $pconfig
# Version=":VN:"
# n |
#	:*:dry run
#	:de:Trockenlauf
# :dir |
#	:*:install directory
#	:de:Installationsverzeichnis
# ::tg |
#	:*:make target 'all' or 'clean'
#	:de:make Ziel 'all' oder 'clean'

usage ()
{
	efeuman -- $0 $1 || printf "$msg1"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

default=""
makeopt=""

while getopts "n" opt
do
	case $opt in
	n)	makeopt="$makeopt -n";;
	\?)	usage -?; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

case "$#" in
2)	default="$2";;
1)	;;
*)	usage -?; exit 1;;
esac

lib=$1

(
printf 'all::\n\n'
printf 'clean::\n\n'

find . -type f | while read path
do
	name=`basename $path`

	case "$name" in
	*.msg)	dir="messages";;
	*.hlp)	dir="help";;
	*.cnf)	dir="config";;
	*)	continue;;
	esac

	case `dirname $path` in
	*/de)	pre="de/";;
	*)	pre="";
	esac

	tg="$lib/$pre$dir/$name"
	printf 'all:: %s\n\n' "$tg"
	printf 'clean::\n\trm -f %s\n\n' "$tg"
	printf '%s: %s\n' "$tg" "$path"
	printf '\t@mkdir -p $(@D)\n\tcp $< $@\n\n' "$tg" "$path"
done
) | make $makeopt -f - $default
