#!/bin/sh
# :*:configuration tool for efeu
# :de:Konfigurationswerkzeug für efeu
#
# $Copyright (C) 2002, 2008 Erich Frühstück
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

# $pconfig
# Version=":VN:"
# v|
#	:*:display this help and exit
#	:de:Fehlermeldungen ausgeben
# x|
#	:*:execute compiled command
#	:de:Kompiliertes Programm ausführen
# s|
#	:*:read function body from standard input
#	:de:Funktionsrumpf von der Standardeingabe lesen
# t:name|
#	:*:top directory of project (default: current working directory)
#	:de:Hauptverzeichnis des Pojekts (Vorgabe: aktuelles Arbeitsverzeichnis)
# c:name|
#	:*:C-Compiler
#	:de:C-Kompiler zum Testen
# i:hdr|
#	:*:header for source
#	:de:Headerfile einbinden
# I:hdr|
#	:*:header for source if exists
#	:de:Headerfile, falls existiert, einbinden
# r:name|
#	:*:include file with additional make parameters
#	:de:Einbindedatei mit zusätzlichen Parametern für make
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
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

tmp=`mktemp -d 2>/dev/null` || tmp=/tmp/efeu$$
trap "rm -rf $tmp" 0
trap "exit 1" 1 2 3

#	Set convention

case "$0" in
*.sh)	cmd="sh $0";;
*)	cmd="$0";;
esac

#	Parse command args

flags=""
rules=""
verbose=0
execute=0
stdin=0
incl=""
cwd=`pwd`
top="$cwd"
eval="$cmd"

while getopts "hvxsc:t:f:r:I:i:" opt
do
	case $opt in
	v)	verbose=1; eval="$eval -v";;
	x)	execute=1;;
	s)	stdin=1;;
	c)	CC="$OPTARG";;
	t)	top="$OPTARG"; eval="$eval -t $OPTARG";;
	f)	flags="$flags $OPTARG";;
	r)	rules="$rules
include $OPTARG";;
	I)	incl="$incl
`$0 include $OPTARG`";;
	i)	incl="$incl
#include <$OPTARG>";;
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

#	Determine C compiler and command invocation

if [ "$CC" != "" ]; then
	case "$CC" in
	clang)	CC="$CC -Qunused-arguments";;
	esac
elif	gcc --version >/dev/null 2>&1; then
	CC="gcc -fPIC -Wall -Wmissing-prototypes -Wpointer-arith"
else
	case `uname -s` in
	AIX)	CC="xlc -qsrcmsg";;
	*)	CC="cc";;
	esac
fi

#	Determine test

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
# Speciel keywords are subsituted by predefined values:
# :de:Einzelne Schlüsselwörter werden durch vordefinierte Parameter ersetzt:
#
# |@@TOP@@|
#	:*:The name of the top directrory (option -t).
#	:de:Der Name des Hauptverzeichnisses (Option -t).
#
# |@@CWD@@|
#	:*:The name of the current working directory.
#	:de:Der Name des aktuellen Arbeitsverzeichnisses.
#
# |@@SRC@@|
#	:*:The name of the template file.
#	:de:Der Name der Schablonendatei.
#
# |@@CMD@@|
#	:*:the name of the script |$!| for recurisive calling.
#	:*:Der Name dieses Skripts |$!| für einen rekursiven Aufruf.
#
# :*:
# The following keywords at start of a line are accepted:
# :de:
# Die folgenden Schlüsselwörter am Anfang der Datei werden akzeptiert:
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
	exec awk -v top="$top" -v cfg_eval="$eval" -v cwd="$cwd" '
BEGIN { pipe = "" }
/@TOP@/ { gsub("@TOP@", top) }
/@CWD@/ { gsub("@CWD@", cwd) }
/@CMD@/ { gsub("@CMD@", cfg_eval) }
/@SRC@/ { gsub("@SRC@", FILENAME) }
/^@include/ { gsub("^@include[ 	]*", "");
	if (!system(cfg_eval flags " include " $0)) flags = flags " -i " $0;
	next }
/^@set/ { gsub("^@set", ""); flags = $0; next }
/^@add/ { gsub("^@add", flags); flags = $0; next }
/^@eval/ { gsub("^@eval", cfg_eval flags); system($0); next }
/^@beg/ { gsub("^@beg", cfg_eval " -s " flags); pipe = $0; next }
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
#	und erneuert <tg>, falls sich das Ergebnis von ihr unterscheidet.
#	oder <src> neueres Datum hat als <tg>.

	if [ -f $2 ]; then
		mkdir -p $tmp || exit 1
		name=`basename $2`
		$eval file $1 > $tmp/$name
		cmp -s $tmp/$name $2 || cp -v $tmp/$name $2
		rm -rf $tmp
	else
		$eval file $1 > $2
	fi

	exit 0
	;;
# $Description
# |cc|
#	:*:returns the used C compiler with the default flags.
#	:de:Gibt den verwendeten C-Compiler mit default-Flags aus.
cc)
	echo $CC
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
ldflags)
# $Description
# |ldflags| <name>
#	:*:links again <name> if availabel.
#	:de: Verwendet Bibliothek <name> ein, falls verfügbar.
	flags="$flags $*"
	success="echo $*"
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
and)
# $Description
# |and| <expr1> <expr2> ...
#	:*:evalate expressions until one is false
#	:de:Ausdrücke auswerten bis der erste falsch ergibt
	for x
	do
		$eval $x || exit 1
	done
	;;
or)
# $Description
# |or| <expr1> <expr2> ...
#	:*:evalate expressions until one is true
#	:de:Ausdrücke auswerten bis der Erste wahr ergibt
	for x
	do
		$eval $x && break
	done
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

mkdir -p $tmp || exit 1
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
	$cmd file >> $tmp/tprog.c
fi

cat >> $tmp/tprog.c <<EOF
$body
return 0;
}
EOF

cat >> $tmp/Makefile <<EOF
TOP=$top

CC=$CC
CPPFLAGS=
CFLAGS=
LDFLAGS=

all::
clean::

$rules

all:: tprog

tprog: tprog.c
	\$(CC) -o \$@ \$< \$(CPPFLAGS) \$(CFLAGS) $flags \$(LDFLAGS)
EOF

if	[ $verbose -gt 0 ]; then
	echo "=== build ===" >&2
	cat $tmp/Makefile >&2
	echo "=== src ===" >&2
	cat $tmp/tprog.c >&2
fi

make -C $tmp 2> $tmp/Error > $tmp/Log
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
# \mref{awk(1)},
# \mref{cc(1)}.
