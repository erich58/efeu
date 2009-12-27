#!/bin/sh
# :*:configuration tool for EFEU
# :de:Konfigurationswerkzeug für EFEU
#
# $Copyright (C) 2002 Erich Frühstück
# This file is part of EFEU.
# 
# EFEU is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
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

#	message formats

msg1="usage: $0 [--help] [opt] cmd arg(s)\n"
msg2="$0: unknown command %s\n"

case ${LANG:=en} in
de*)
	msg1="Aufruf: $0 [--help] [opt] cmd arg(s)\n"
	msg2="$0: Unbekanntes Kommando %s\n"
	;;
esac

# $alias
# $pconfig
# Version="$Id: efeuconfig.sh,v 1.10 2008-04-06 18:52:51 ef Exp $"
# v|
#	:*:display this help and exit
#	:de:Fehlermeldungen ausgeben
# x|
#	:*:execute compiled command
#	:de:Kompiliertes Programm ausführen
# s|
#	:*:read function body from standard input
#	:de:Funktionsrumpf von der Standardeingabe lesen
# c:name|
#	:*:C-Compiler
#	:de:C-Kompiler zum Testen
# i:hdr|
#	:*:header for source
#	:de:Headerfile einbinden
# I:hdr|
#	:*:header for source if exists
#	:de:Headerfile, falls existiert, einbinden
# r:hdr|
#	:*:header for shmkmf
#	:de:Headerfile für shmkmf einbinden
# f:flgs|
#	:*:compiler flags
#	:de:Flags für C-Kompiler
# :cmd|
#	:*:command to perform
#	:de:Auszuführendes Kommando
# ::arg(s)|
#	:*:command specific arguments
#	:de:Befehlsspezifische Argumente

usage ()
{
	efeuman -- $0 $1 || printf "$msg1"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version="[$]Id:' $0; exit 0;;
esac

tmp=/tmp/efeu$$
trap "rm -rf $tmp" 0
trap "exit 1" 1 2 3

CC=efeucc
flags=""
verbose=0
execute=0
stdin=0
shmkmf=0
smh_flags=
mkmf=0
eval="$0"
incl=""
rules=""

#	parse command args

while getopts "hvxsc:f:I:i:r:" opt
do
	case $opt in
	v)	verbose=1; eval="$eval -v";;
	x)	execute=1;;
	s)	stdin=1;;
	c)	CC="$OPTARG";;
	f)	flags="$flags $OPTARG";;
	I)	incl="$incl
`$0 include $OPTARG`";;
	i)	incl="$incl
#include <$OPTARG>";;
	r)	shmkmf=1; eval="$eval -r $OPTARG"; rules="$rules
include $OPTARG";;
	\?)	usage -?; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

if	[ $# -lt 1 ]
then	usage -?; exit 1
fi

decl=""
body=""
success=":"
failure=":"
type=$1
shift

#	determine test

# $Description
#
# :*:
# The command |$!| is used to determine system specific parameters.
# The normal use is to to process a C header template with special
# test instructions. The instructions are isolated by awk and
# are evaluated by a call to |$!|.
# :de:
# Das Kommando |$!| dient zur Bestimmung von systemspezifischen Parametern.
# Normalerweise wird damit eine Schablone für ein C-Headerfile 
# mit speziellen Testanweisungen überarbeitet.
# Diese Anweisungen werden mit awk isoliert und von |$!| verarbeitet.
#
# :*:
# Some tests create c-sources and checks the compilation status.
# If the |-x| flag is set, the created program is executed and
# its output is inserted.
# :de:
# Manche Tests generieren C-Sourcen und überprüfen den Kompilierungsstatus.
# Falls die |-x| Option gesetzt ist, wird das generierte Programm auch
# ausgeführt und seine Ausgabe eingearbeitet.
#
# @arglist -i
# 
# :*:
# The first argument determines the action of |$!|. If it is |file|
# or |update|, a template file is evaluated, otherwise a special
# test is performed.
# :de:
# Das erste Argument bestimmt die Aktivitäten von |$!|. Fals es |file|
# oder |update| ist, wird eine Schablonendatei verarbeitet, ansonsten wird
# ein spezieller Test durchgeführt.
#
# :*:
# Every occurance of |@@SRC@@| in the template file would be replaced by
# the name of the template file and |@@CMD@@| by |$!|. The following keywords
# at start of a line are accepted:
# :de:
# In der Schablonendatei wird jedes Vorkommen von |@@SRC@@| durch den
# Namen der Schablonendatei und |@@CMD@@| durch |$!| ersetzt. Die folgenden
# Schlüsselwörter am Anfang der Datei werden akzeptiert:
#
# |@@set| <flags>
#	:*:sets options for the following calls to |$!|.
#	:de:Setzt Optionen für die nachfolgenden Aufrufe von |$!|.
#
# |@@add| <flags>
#	:*:expands the options for the following calls to |$!|.
#	:de:Erweitert die Optionen für die nachfolgenden Aufrufe von |$!|.
#
# |@@include| <file>
#	:*:if header <file> exists, a include directive is createted and the
#	options for the following calls to |$!| are expanded to include it.
#	:de:Falls die Headerdatei <file> existiert, wird eine
#	include-Direktive generiert und die Optionen für
#	die nachfolgenden Aufrufe von |$!| erweitert, um sie einzubinden.
#
# |@@eval| <args>
#	:*:calls |$!| with the given arguments.
#	:de:Ruft |$!| mit den angegebenen Argumenten <args> auf.
#
# |@@xlib| <args>
#	:*:calls |efeu-xlib| with the given arguments.
#	:de:Ruft |efeu-xlib| mit den angegebenen Argumenten <args> auf.
#
# |@@beg| <args>\br
# ...\br
# |@@end|
#	:*:calls |$!| with |-s| and the the given arguments.
# 	All lines between |@@beg| and |@@end| are send to |$!|.
#	:de:Ruft |$!| mit Option |-s| und den angegebenen Argumenten auf und
#	leitet alle Zeilen zwischen |@@beg| und |@@end| an |$!| weiter.
#
# :*:The following commands are performed by the command |$!|
# :de:Die folgenden Kommandos werden vom Programm |$!| ausgeführt:

case $type in
file)
# $Description
# |file| [<name>]
#	:*:inserts system specific parameters in file <name>
#	:de:Fügt in Datei <name> systemspezifische Parameter ein
	exec awk -v "eval=$eval" '
BEGIN { pipe = "" }
/@SRC@/ { gsub("@SRC@", FILENAME) }
/@CMD@/ { gsub("@CMD@", eval) }
/^@include/ { gsub("^@include[ 	]*", "");
	if (!system(eval " include " $0)) flags = flags " -i " $0;
	next }
/^@set/ { gsub("^@set", ""); flags = $0; next }
/^@add/ { gsub("^@add", flags); flags = $0; next }
/^@eval/ { gsub("^@eval", eval flags); system($0); next }
/^@xlib/ { gsub("^@xlib", "efeu-xlib"); system($0); next }
/^@beg/ { gsub("^@beg", eval " -s " flags); pipe = $0; next }
/^@end/ { close(pipe); pipe = ""; next }
/^@@/ { gsub("^@@", "@") }
{ if (pipe) print | pipe; else print }
' $*
	;;
update)
# $Description
# |update| <src> <tg>
#	:*:performs $! file on <src> and overwrites <tg> only if
#	<src> is newer than <tg> or <tg> has changed.
#	:de:Fügt in die Datei <src> systemspezifische Parameter ein
#	und erneuert <tg>, falls sich das Ergebniss von ihr unterscheidet
#	oder <src> neueres Datum hat als <tg>.
	mkdir $tmp || exit 1
	name=`basename $2`
	$eval file $1 > $tmp/$name
	
	if	cmp -s $tmp/$name $2
	then	echo "$2: $1; cp $tmp/$name $2" > $tmp/Makefile
	else	echo "$2: $tmp/$name; cp $tmp/$name $2" > $tmp/Makefile
	fi

	make -f $tmp/Makefile
	rm -rf $tmp
	exit 0;
	;;
check)
# $Description
# |check| <expr>
#	:*:check compilation/execution of <expr> without any additional	message.
#	:de:Testet Kompilierung/Ausführung von <expr> ohne zusätzliche Ausgabe.
	body="$*;"
	;;
report)
# $Description
# |report| <expr>
#	:*:reports status of compilation/execution of <expr>.
#	:de:Meldet den Status der Kompilierung/Ausführung von <expr>.
	success="echo success"
	failure="echo failure"
	body="$*;"
	;;
error)
# $Description
# |error| <message> <expr>
#	:*:create |#error| directive with <message>,
#	if compilation/execution fails.
#	:de:Generiert |#error| Directive mit <message>, falls die
#	Kompilierung/Ausführung von <expr> fehlschlägt.
	failure="echo #error $1"
	shift
	body="$*;"
	;;
success)
# $Description
# |success| <tname> <expr>
#	:*:if compilation/execution of <expr> is successful,
#	define <tname> with 1, else with 0.
#	:de:Falls die Kompilierung/Ausführung von <expr> erfolgreich ist,
#	wird der Makro <tname> mit 1, ansonsten mit 0 definiert.
	success="echo #define $1 1"
	failure="echo #define $1 0"
	shift
	body="$*;"
	;;
failure)
# $Description
# |failure| <tname> <expr>
#	:*:if compilation/execution of <expr> failed,
#	define <tname> with 1, else with 0.
#	:de:Falls die Kompilierung/Ausführung von <expr> fehlschlägt, wird der
#	Makro <tname> mit 1, ansonsten mit 0 definiert.
	success="echo #define $1 0"
	failure="echo #define $1 1"
	shift
	body="$*;"
	;;
include)
# $Description
# |include| <name>
#	:*:includes header <name> if availabel.
#	:de:Bindet header <name> ein, falls verfügbar.
	incl="$incl
#include <$1>"
	success="echo #include <$1>"
	;;
proto)
# $Description
# |proto| <proto>
#	:*:check compatibility of prototype <proto>.
#	:de:Testet die Kompatiblität des Prototypes <proto>.
	success="echo $*;"
	failure="echo prototype not compatible >&2"
	decl="$*;"
	;;
typedef)
# $Description
# |typedef| <type> <decl>
#	:*:if <type> is not defined, declare it with <decl>.
#	:de:Falls der Type <type> nicht deklariert ist, wird
#	er mit <decl> <type> definiert.
	name=$1; shift
	failure="echo typedef $* $name;"
	body="$name x;"
	;;
*)
	printf "$msg2" $type
	awk '
BEGIN { flag = 0 }
/^case .type in/ { flag = 1 }
/^esac/ { flag = 0 }
#flag && /^# \|/ { gsub("^# \| | [|]", ""); gsub("\|", ""); print }
flag && /^# \|/ { gsub("^# |[|]", ""); print }
' $0
	exit 1
	;;
esac

#	run test

mkdir $tmp || exit 1
cat > $tmp/tprog.c <<EOF
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
$incl
$decl

int main (int argc, char **argv)
{
EOF

if [ $stdin -ne 0 ]; then
	$0 file >> $tmp/tprog.c
fi

cat >> $tmp/tprog.c <<EOF
$body
return 0;
}
EOF

if	[ $verbose -gt 0 ]; then
	echo "=== src ===" >&2
	cat $tmp/tprog.c >&2
fi

if [ $shmkmf -ne 0 ]; then
	cat > $tmp/Config.make << EOF
include cc.smh
$rules

mf_cc -o tprog tprog.c $flags
EOF
	if	[ $verbose -gt 0 ]; then
		echo "=== rules ===" >&2
		cat $tmp/Config.make >&2
	fi

	(cd $tmp; shmkmf; make) 2> $tmp/Error > $tmp/Log
else
	if	[ $verbose -gt 0 ]; then
		echo "=== build ===" >&2
		echo $CC $flags -o tprog tprog.c >&2
	fi

	(cd $tmp; $CC $flags -o tprog tprog.c) 2> $tmp/Error > $tmp/Log
fi

stat=1

if	[ -x $tmp/tprog ]
then
	if	test $execute -eq 0 || $tmp/tprog
	then
		$success
		stat=0
	else
		$failure
	fi
else
	$failure
fi

if	[ $verbose -gt 0 ]; then
	echo "=== log ===" >&2
	cat $tmp/Log >&2
	echo "=== err ===" >&2
	cat $tmp/Error >&2
	echo "=== end ===" >&2
fi

rm -rf $tmp
exit $stat

# $SeeAlso
# \mref{efeu-xlib(1)}.
