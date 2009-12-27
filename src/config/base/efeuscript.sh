#!/bin/sh
# :*:install script file
# :de:Installation von Scripts
#
# Copyright (C) 2001 Erich Frühstück
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

# message formats

: ${LANG:=en}

fmt_usage="
usage: $0 [-h] [-uger] [-c cmd] src target

-h	this output
-u	only user gets exec permission
-g	only user and group gets exec permission
-e	use /usr/bin/env to start interpreter
-r	use exec to start interpreter
-c name	name of interpreter, default $cmd

The command completes a script with a interpreter key (#!).
The complete path of the interpreter is discoverd.

If the script contains already an interpreter key, it is
only checked and not modified.
"
fmt_nosrc="$0: file %s does not exist!\n"
fmt_nocmd="$0: command %s not found!\n"
fmt_notin="\tnot in %s\n"

case $LANG in
de*)
	fmt_usage="
Aufruf: $0 [-h] [-uger] [-c cmd] src target

-h	Dieser Text
-u	Nur Eigentümer erhält Ausführungsrechte
-g	Nur Eigentümer und Gruppe erhalten Ausführungsrechte
-e	/usr/bin/env zum Start des Interpreters verwenden
-r	exec zum Start des Interpreters verwenden
-c name	Name des Befehlsinterpreters, Vorgabe $cmd

Das Kommando ergänzt ein Skript mit einer Interpreterkennung.
Dabei wird der vollständige Pfad des Interpreters automatisch
ermittelt.

Falls bereits eine Interpreterkennung vorhanden ist, wird diese
nur geprüft und nicht verändert.
"
	fmt_nosrc="$0: Datei %s existiert nicht!\n"
	fmt_nocmd="$0: Kommando %s nicht gefunden!\n"
	fmt_notin="\tNicht in %s\n"
	;;
esac

# set default shell

cmd=/bin/sh

for x in /bin/bash /bin/ksh
do
	if [ -x $x ]; then
		cmd=$x;
		break
	fi
done

# parse command args 

mode=a+x
use_exec=0
use_env=0
check_ext=1

while getopts hugerc: opt
do
	case $opt in
	u)	mode="u+x,og-x";;
	g)	mode="ug+x,o-x";;
	e)	use_env=1;;
	r)	use_exec=1;;
	c)	cmd="$OPTARG"; check_ext=0;;
	h)	echo "$fmt_usage"; exit 0;;
	\?)	echo "$fmt_usage" >&2; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

if [ $# -ne 2 ]; then
	echo "$fmt_usage"
	exit 1
elif [ ! -f $1 ]; then
	printf "$fmt_nosrc" $1 >&2
	exit 1
fi

# function to get absolut path of command

getpath ()
{
	IFS=':'

	for x in $PATH
	do
		if [ -x $x/$1 ]; then
			echo $x/$*
			return
		fi
	done

	printf "$fmt_nocmd" $1 >&2

	for x in $PATH
	do
		printf "$fmt_notin" $x >&2
	done

	exit 1
}

# check/add interpreter key

head=`sed -e 1q $1`

if [ $check_ext != 0 ]; then
	case $1 in
	*.esh)	cmd=esh; use_env=1;;
	*.wish)	cmd=wish; use_env=1;;
	esac
fi
	
if
	path=`expr "$head" : '#![ 	]*\([^ 	]*\).*'`
then
	if	[ ! -x $path ]; then
		printf "$fmt_nocmd" "$path" >&2
		exit 1;
	fi

	cp $1 $2 || exit 1
elif
	test $use_env -ne 0
then
	path=`getpath env` || exit 1
	echo "#!$path $cmd" > $2
	cat $1 >> $2
elif
	test $use_exec -ne 0
then
	echo "#!/bin/sh" > $2
	echo "#* the next line restarts using $cmd \\" >> $2
	echo "exec $cmd" '"$0" "$@"' >> $2
	echo >> $2
	cat $1 >> $2
else
	case $cmd in
		esh)
			path=`getpath env` || exit 1
			echo "#!$path $cmd" > $2
			echo >> $2
			;;
		/*)
			if	[ -x $cmd ]; then
				echo "#!$cmd" > $2
			else
				printf "$fmt_nocmd" $cmd >&2
				exit 1;
			fi
			;;

		*)	
			path=`getpath $cmd` || exit 1
			echo "#!$path" > $2
			;;
	esac
			
	cat $1 >> $2
fi

chmod $mode $2
