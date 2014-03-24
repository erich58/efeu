#!/bin/sh
# :*:create Makefile from shell
# :de:Makefile mithilfe der Shell generieren
#
# $Copyright (C) 2004 Erich Frühstück
# This file is part of EFEU.
# 
# EFEU is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
# 
# EFEU is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public
# License along with EFEU; see the file COPYING.
# If not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

# $pconfig
# Version=":VN:"
# Config=Config.make
# Makefile=Makefile
#
# v |
#	:*:write extra comments to the makefile
#	:de:Zusätzliche Kommentare ins Makefile schreiben
# i |
#	:*:read commands from stdin
#	:de:Lese Befehle aus der Standardeingabe
# p |
#	:*:write rules to stdout
#	:de:Generierte Regeln zur Standardausgabe schreiben
# c:string |
#	:*:read commands from <string>
#	:de:Lese Befehle aus der Zeichenkette <string>.
# x |
#	:*:do not create extra rules and comments
#	:de:Keine zusätzlichen Regeln und Kommentare generieren
# r:arg |
#	:*:create Makefile in a temporary directory and run |make|
#	with the given argument <arg>.
#	:de:Makefile in einem temprären Verzeichnis generieren und
#	|make| mit gegebenem Argument <arg> aufrufen.
# t:top |
#	:*:sets |TOP| to <top> (default: .)
#	:de:Setzt |TOP| auf <top>, Vorgabe: .
# C:dir |
#	:*:expand config path with <dir>
#	:de:Suchpfad für Konfigurationsdateien um <dir> erweitern
# I:dir |
#	:*:expand include path with <dir>
#	:de:Suchpfad für Headerdateien um <dir> erweitern
# ::config | Config
#	:*:name of configuration file (default: {Config})
#	:de:Name der Konfigurationsdatei, Vorgabe: {Config}
# ::makefile | Makefile
#	:*:name of target file (default: {Makefile})
#	:de:Name des generierten Makefiles, Vorgabe: {Makefile}

_shmkmf_usage ()
{
	efeuman -- $0 $1 || cat << EOF
usage: $0 [-vpx] [-r arg] [-I dir] [-t top] config makefile
EOF
}

case "$1" in
-\?|--help*)	_shmkmf_usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

# set search path

shmkmf_top=`dirname $0`

case "$shmkmf_top" in
	*/bin | bin)	shmkmf_top=`dirname $shmkmf_top`;;
esac

IncludePath=$shmkmf_top

for _x in $shmkmf_top /usr/local /usr
do
	_y="$_x/lib/shmkmf"

	if [ -d $_y ]; then
		IncludePath=$_y
		break;
	fi
done

ConfigPath=$shmkmf_top

for _x in $shmkmf_top /usr/local /
do
	_y="$_x/etc/shmkmf"

	if [ -d $_y ]; then
		ConfigPath=$_y
		break;
	fi
done

# function returning quoted string

shmkmf_quote ()
{
	echo "$*" | sed -e "s/'/'\\\\''/g" -e "s/^/'/g" -e "s/$/'/g"
}

# parse command arguments

case "$0" in
*.sh)	shmkmf="sh $0";;
*)	shmkmf="$0";;
esac

TOP=.
Config=Config.make
Makefile=Makefile
bootstrap="$shmkmf"

_cmd=
_to_stdout=0
_tmp_build=0
_autorun=0
_make_arg=
_stdrules=true
shmkmf_verbose=

while getopts vpxic:r:t:C:I: opt
do
	case $opt in
	\?)	_shmkmf_usage; exit 1;;
	v)	shmkmf_verbose=1; bootstrap="$bootstrap -v";;
	p)	_tmp_build=1; _to_stdout=1;;
	x)	_stdrules=;;
	i)	_cmd="cat";;
	r)	_tmp_build=1; _autorun=1; _make_arg="$OPTARG";;
	t)	TOP="$OPTARG"; bootstrap="$bootstrap -t $TOP";;
	c)
		_x="`shmkmf_quote $OPTARG`";
		_cmd="echo $_x";
		bootstrap="$bootstrap -c $_x"
		;;
	C)	
		if	test A$OPTARG = A-
		then	ConfigPath=""
		else	ConfigPath="$OPTARG:$ConfigPath"
		fi
		bootstrap="$bootstrap -C$OPTARG"
		;;
	I)	
		if	test A$OPTARG = A-
		then	IncludePath=""
		else	IncludePath="$OPTARG:$IncludePath"
		fi
		bootstrap="$bootstrap -I$OPTARG"
		;;
	esac
done

shift `expr $OPTIND - 1`

if [ "$#" -gt 2 ]; then
	_shmkmf_usage
	exit 1
fi

shmkmf_id=`basename $0`

# set locale specific values

: ${LANG:=en}

language=`expr "$LANG" : "\([a-z][a-z]\).*"`
: ${language:=en}

# codeset not supported yet
#codeset=`expr "$LANG" : ".*[.]\([^@]*\).*"`

# Die Funktion $1 spaltet eine Pfadvariable auf und fügt bei //
# alle möglichen Zwischenverzeichnisse mithilfe von find ein.
# Das Startverzeichnis für find enthält immer einen nachgestellen
# Schrägstrich, damit als Startverzeichnis ein symbolischer Link
# angegeben werden kann. Nicht alle von $1 gelieferten Verzeichnisse
# existieren.

splitpath ()
{(
	IFS=":"
	for d in $1
	do
		case "$d" in
		?*//?*/*)
			a=`expr $d : '\(.*/\)/'`
			b=`expr $d : '.*//\([^/]*\)'`
			c=`expr $d : '.*//[^/]*\(/.*\)'`
			find -L $a -type d -name "$b" \
				-exec test -d \{\}$c \; \
				-print 2>/dev/null |\
				sed -e "s|\$|$c|"
			;;
		?*//?*)
			a=`expr $d : '\(.*/\)/'`
			b=`expr $d : '.*//\(.*\)'`
			find -L $a -type d -name "$b" -print 2>/dev/null
			;;
		*//)	
			a=`expr $d : '\(.*/\)/$'`
			find -L $a -type d -print 2>/dev/null | sed -e 's|/$||'
			;;
		*)	
			printf "%s\n" $d
			;;
		esac
	done
) | LANG=C sort
}

# Absoluten Dateinamen bestimmen.

_shmkmf_fsearch ()
{
	while read dir
	do
		if [ -r $dir/$1 ]
		then
			printf -- "%s\n" $dir/$1
			return 0
		fi
	done

	return 1
}

# search file according to path

fsearch ()	# usage: fsearch path file
{
	[ $# -eq 2 ] || return 1

	case "$2" in
	[./]*)
		if [ -f "$2" ]; then
			printf "%s\n" "$2"
			return 0
		elif [ $_tmp_build -eq 1 -a -f $SRC/$2 ]; then
			printf "%s\n" "$SRC/$2"
			return 0
		fi

		return 1
		;;
	*)
		splitpath "$1" | _shmkmf_fsearch "$2"
		return
		;;
	esac
}

# get message format

getfmt ()
{
	if
		_catalog=`fsearch $IncludePath $1.msg`
	then
		_mode=0
		_fmt="$2"

		while
			read _key _entry
		do
			if [ "$_key" = "*" ]; then
				if [ "$_entry" = "$2" ]; then
					_mode=1
				else
					_mode=0
				fi
			elif [ $_mode = 1 ]; then
				if [ "$_key" = "$language" ]; then
					_fmt="$_entry"
					break
				fi
			fi
		done < $_catalog

		printf "%s\n" "$_fmt"
	else
		printf "%s\n" "$2"
	fi
}

fmt_head=`getfmt shmkmf "Do not edit, file was created with\n# %s"`
fmt_srule=`getfmt shmkmf "Rules added by %s"`

# determine config file 

test "$#" -ge 2 -a "x$2" != "x-" && Makefile=$2
test "$#" -ge 1 -a "x$1" != "x-" && Config=$1

if [ -d $Config ]; then
	SRC=$Config
	Config=$SRC/Config.make
else
	SRC=`dirname $Config`
	Config=$SRC/`basename $Config`
fi

if [ -z "$_cmd" -a ! -f $Config ]; then
	_fmt=`getfmt shmkmf "configuration file %s not defined."`
	printf "$shmkmf_id: $_fmt\n" $Config >&2
	exit 1
fi

# crate directory for temporary files

TMP=${TMPDIR:-/tmp}/mkmf$$
mkdir $TMP || exit 1
chmod go-rwx $TMP
readonly TMP
trap "cd; rm -rf $TMP" 0

if [ $_tmp_build -eq 1 ]; then
	SRC=`cd $SRC; pwd`
	test -n "$_cmd" || Config="$SRC/`basename $Config`"
	TOP=`cd $TOP; pwd`
	Makefile=Makefile
	test -n "$_cmd" || bootstrap="$bootstrap $Config"
	cd $TMP
else
	bootstrap="$bootstrap $*"
fi

readonly shmkmf bootstrap Makefile Config

# set output file

if [ $_to_stdout -eq 1 ]; then
	_output=$TMP/shmkmf.out
	touch $_output || exit 1
else
	_output=$Makefile
fi

readonly _output

shmkmf_abort ()
{
	_fmt=`getfmt shmkmf "Creation of %s aborted."`
	printf "$_fmt\n" "$Makefile" >> $_output
#	_fmt=`getfmt shmkmf "abortion: %s deleted."`
#	printf "$shmkmf_id: $_fmt\n" $_output >&2
#	printf "%s\n" "---- $Makefile ----" >&2
#	cat $_output >&2
#	printf "%s\n" "---- END ----" >&2
#	rm -rf $_output
	if [ $_to_stdout -eq 1 ]; then
		cat $_output >&2
	fi
	exit $1
}

trap "shmkmf_abort 1" 1 2 3

# error functions

shmkmf_error ()
{
	_fmt=`getfmt "$1" "$2"`
	shift 2
	printf "$shmkmf_id: $_fmt\n" "$@" >&2
	shmkmf_abort 1
}


shmkmf_warg ()
{
	_fmt=`getfmt shmkmf "function %s: wrong number of arguments."`

	case "$#" in
	0)	printf "$shmkmf_id: $_fmt\n" "?" >&2;;
	*)	printf "$shmkmf_id: $_fmt\n" "$1" >&2;;
	esac

	shmkmf_abort 1
}

shmkmf_invop ()
{
	_fmt=`getfmt shmkmf "function %s: invalid option %s."`

	case "$#" in
	0)	printf "$shmkmf_id: $_fmt\n" "?" "?" >&2;;
	1)	printf "$shmkmf_id: $_fmt\n" "$1" "?" >&2;;
	*)	printf "$shmkmf_id: $_fmt\n" "$1" "$2" >&2;;
	esac

	shmkmf_abort 1
}

shmkmf_not_found ()
{
	_fmt=`getfmt shmkmf "file %s not found (Path=%s)."`

	case "$#" in
	0)	printf "$shmkmf_id: $_fmt\n" "?" "?" >&2;;
	1)	printf "$shmkmf_id: $_fmt\n" "$1" "?" >&2;;
	*)	printf "$shmkmf_id: $_fmt\n" "$1" "$2" >&2;;
	esac

	shmkmf_abort 1
}

# include files

_deplist=$TMP/.deplist

add_depend ()	# usage: add_depend name(s)
{
	for _x
	do
		echo "$_x" >> $_deplist
	done
}

_shmkmf_depth=0

do_include ()
{
	eval FILE$_shmkmf_depth=$FILE
	_shmkmf_depth=`expr $_shmkmf_depth + 1`
	FILE=$1
	add_depend $FILE
	. $FILE
	_shmkmf_depth=`expr $_shmkmf_depth - 1`
	eval FILE=\$FILE$_shmkmf_depth
	eval FILE$_shmkmf_depth=
}

include ()	# usage: include name
{
	if
		_name=`fsearch $IncludePath $1`
	then
		do_include $_name
	else
		shmkmf_not_found "$1" "$IncludePath"
	fi
}

xinclude ()	# usage: xinclude name [stat]
{
	if
		fsearch $IncludePath $1 > /dev/null
	then
		if [ "$#" -gt 1 ]; then
			_name="$2"
		else
			_name="`basename $1 .smh`.stat"
		fi

		$0 -xp -t $TOP -c "include $1" > $_name
		cat $_name
		printf "mf_update -n %s \"%s -xp -t %s -c 'include %s'\"\n" \
			$_name "$0" $TOP "$1" >> $shmkmf_post
		add_depend $_name
	else
		shmkmf_not_found "$1" "$IncludePath"
	fi
}

config ()	# usage: config name
{
	if
		_name=`fsearch $ConfigPath $1`
	then
		do_include $_name
	else
		return 1
	fi
}

# postponing commands

shmkmf_post=$TMP/shmkmf.post

postpone ()	#usage: postpone cmd arg(s)
{
	_post_cmd="$1"
	shift

	for _post_arg 
	do
		_post_cmd="$_post_cmd '$_post_arg'"
	done

	echo "$_post_cmd" >> $shmkmf_post
}

# create and load config file

mkconfig ()	#usage: mkconfig [-c] [-C clean] cmd arg(s)
{
	_clean=""
	_name=""

	while [ -n "$1" ]
	do
		case "$1" in
		-c)	_clean="$CleanTarget"; shift 1;;
		-C)	_clean="$2"; shift 2;;
		-*)	shmkmf_invop mkconfig "$1";;
		*)	_name="$1"; shift 1; break;;
		esac
	done

	if [ -n "$_clean" ]; then
		postpone printf "\n%s::\n\trm -f %s\n" "$_clean" "$_name" 
	fi

	if [ ! -f $_name ]; then
		printf "# $fmt_head\n\n" "$bootstrap" > $_name
		"$@" >> $_name
	fi
	
	. $_name
}

mf_bootstrap ()
{
	printf "\n%s: " "$Makefile"
	sort -u $_deplist | sed -e '$!s/$/ \\/' -e '1!s/^/  /'
	printf "\t%s\n" "$bootstrap"
	printf "\nupdate::\n\t%s\n" "$bootstrap"
}

# depend run

mf_depend ()	# usage: mf_depend [-d dep] rules
{
	if [ "A$1" = "A-d" ]; then
		shift 1
		mf_rule -d depend "$@"
	else
		mf_rule -d depend "" "$@"
	fi

	touch $TMP/.make_depend
}

# setup parameters

FILE=$0
add_depend "$0"
config shmkmf.cfg

if [ -f $TOP/shmkmf.cfg ]; then
	do_include $TOP/shmkmf.cfg
fi

# process config file

cp /dev/null $_output

include var.smh >> $_output

if [ $_stdrules ]; then
	printf "# $fmt_head\n" "$bootstrap" >> $_output
	mf_var CONFIG $Config >> $_output
	mf_var -n MAKEFILE $Makefile >> $_output
fi

include base.smh >> $_output

if [ "$_cmd" ]; then
	eval $_cmd > $TMP/.input
	do_include $TMP/.input >> $_output
	rm -f $TMP/.input
elif [ -f $Config ]; then
	add_depend $Config
	do_include $Config >> $_output
else
	_fmt=`getfmt shmkmf "configuration file %s not defined."`
	printf "$shmkmf_id: $_fmt\n" $Config >&2
	shmkmf_abort 1
fi

# add standard rules

if [ $_stdrules ]; then
	printf "\n# $fmt_srule\n" $shmkmf_id >> $_output
	mf_rule -d "all" >> $_output
	mf_bootstrap >> $_output
	mf_rule -d "install" "all" >> $_output
	mf_rule -d "depend" >> $_output
	mf_rule -d "purge" "" "rm -f core" >> $_output
	mf_rule -d "clean" "purge" >> $_output
	mf_rule -d "uninstall" >> $_output
fi

# execute postponed commands

while [ -f $shmkmf_post ]
do
	if [ $shmkmf_verbose ]; then
		_fmt=`getfmt shmkmf "Postponed rules"`
		printf "\n# $_fmt\n" >> $_output
		sed -e 's/^/# /' $shmkmf_post >> $_output
	fi

	awk '{ printf "%07d:%s\n", NR, $0 }' $shmkmf_post | \
		sort -r | cut -d: -f2- > $shmkmf_post.r
	rm -f $shmkmf_post
	. $shmkmf_post.r >> $_output
	rm -f $shmkmf_post.r
done

# postprocessing

if [ $_to_stdout -eq 1 ]; then
	cat $_output
	rm -rf $_output
elif [ -f $TMP/.make_depend ]; then
	printf "\n# $fmt_srule\n" "make depend" >> $Makefile
	make -f $Makefile depend
fi

if [ $_autorun -eq 1 ]; then
	make $_make_arg
fi
