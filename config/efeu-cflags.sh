#!/bin/sh
# :*:	determine c-flags for use of external libraries
# :de: 	Bestimmung von C-Flags zur Einbindung externer Bibliotheken
#
# $Copyright (C) 2019 Erich Frühstück
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

#	message formats

msg1='usage: %s [--help] name [-Ipath] [-Lpath] [-ao] [-lname | hdr]\n'
msg2='%s: invalid option %s.\n'

case ${LANG:=en} in
de*)
	msg1='Aufruf: %s [--help] name [-Ipath] [-Lpath] [-ao] [-lname | hdr]\n'
	msg2='%s: Ungültige Option %s.\n'
	;;
esac

# $pconfig !
# :name |
#	:*:name <name> of macro with found flags.
#	:de:Name <name> des Makros für die gefundenen Flags.
# I::path |
#	:*:additionaly path to search header files.
#	:de:Zusätzlicher Suchpfad für Kopfdateien.
# L::path |
#	:*:additionaly path to search libraries.
#	:de:Zusätzlicher Suchpfad für Bibliotheken.
# a |
#	:*:try next search only if last search was successfull.
#	:de:Nächste Suche nur durchführen, wenn letzte Suche erfolgreich war.
# o |
#	:*:try next search only if last search failed.
#	:de:Nächste Suche nur durchführen, wenn letzte Suche fehlschlug.
# l:lib |
#	:*:search for libraray corresponding to the regular
#	expression |^lib(|<lib>|)\.(a||so)$$|. On success, print out
#	the |-l|<name> flag for linking the library. If the library is
#	not in the default place of the compiler an |-L|<dir> flag is also
#	generated.
#	:de:Suche nach der Bibliothek, die dem regulären
#	Ausdruck |^lib(|<lib>|)\.(a||so)$$| entspricht. Liefert bei
#	Erfolg das entsprechende |-l| flag und falls die Bibliothek
#	nicht im Standarsuchpfad des Kompilers liegt,
#	optional auch ein eintsprechendes |-L|<dir> flag. 
# ::hdr |
#	:*:search directory <dir> which contains header <hdr>, where <hdr> may
#	contain directory parts as in |X11/X.h|. If <dir> is different from
#	|/usr/include|, print out the option |-I|<dir>.
#	:de:Suche nach dem Verzeichnis <dir>, welches die Kopfdatei <hdr>
#	enthält, wobei <name> Pfadteile enthalten kann (z.B: |X11/X.h|)
#	Falls <dir> verschieden von |/usr/include| ist, wird ein Kompilerflag
#	der Form |-I|<dir> erzeugt.
# !

usage ()
{
	efeuman -- $0 $1 || printf "$msg1" "$0"
}

STDLIB="/lib /usr/lib"

# Pfad aufspalten

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
				sed -e "s|\$|$c|" | LANG=C sort
			;;
		?*//?*)
			a=`expr $d : '\(.*/\)/'`
			b=`expr $d : '.*//\(.*\)'`
			find -L $a -type d -name "$b" -print 2>/dev/null |
				LANG=C sort
			;;
		*//)	
			a=`expr $d : '\(.*/\)/$'`
			find -L $a -type d -print 2>/dev/null |
				sed -e 's|/$||' | LANG=C sort
			;;
		*)	
			printf "%s\n" $d
			;;
		esac
	done
)
}

expandpath ()
{
	splitpath `expr "$*" : '..\(.*\)'`
}

# Dateinamen eines Shared Objects bestimmen.

soname ()
{
	name=$1
	shift

	for x
	do
		if [ -f $x/$name ]; then
			printf " %s" $x/$name
			return 0
		fi
	done

	for x in $STDLIB
	do
		if [ -f $x/$name ]; then
			printf " %s" $x/$name
			return 0
		fi
	done

	return 1
}

# Include-Verzeichnis für Header bestimmen.

cppflags ()
{
	name=$1
	shift

	for x
	do
		if [ -f $x/$name ]; then
			printf -- " -I%s" $x
			return 0
		fi
	done

	for x in /usr/include
	do
		if [ -f $x/$name ]; then
			return 0
		fi
	done

	return 1
}

# Link-Flags bestimmen

ldflags ()
{
	while read name
	do
		case "$name" in
		*.a)	lib=`expr "$name" : '.*/lib\(.*\).a$'`;;
		*.so)	lib=`expr "$name" : '.*/lib\(.*\).so$'`;;
		*)	continue;
		esac

		case "$1" in
		-L)	printf -- " -L%s -l%s" `dirname $name` "$lib";;
		*)	printf -- " -l%s" "$lib";;
		esac

		return 0
	done

	return 1
}

find_lib ()
{
	name=`expr "$1" : '..\(.*\)'`
	shift

	find $* -maxdepth 1 -name "lib$name.*" | sort |
		ldflags -L && return 0

	find $STDLIB -maxdepth 1 -name "lib$name.*" | sort |
		ldflags && return 0

	return 1
}

# :*:
# The function $! creates c-flags for use of external libraries.
# The following options and arguments are accepted:
# :de:
# Die Funktion $! bestimmt C-Flags zur Einbindung externer Bibliotheken.
# Die folgenden Optionen und Argumente werden akzeptiert:
#
# @arglist

case "$1" in
-*)
	usage $1; exit 0;;
?*)	printf '%s =' "$1";;
*)	usage -?; exit 0;;
esac

ipath=
lpath=

while [ -n "$1" ]
do
	case "$1" in
	-a)	shift; [ $last -ne 0 ] && shift;;
	-o)	shift; [ $last -eq 0 ] && shift;;
	-I*)	ipath="$ipath `expandpath $1`"; shift;;
	-L*)	lpath="$lpath `expandpath $1`"; shift;;
	-l*)	find_lib "$1" $lpath; last=$?; shift;;
	-*)	printf "$msg2" "$0" "$1"; exit 1;;
	*.so)	soname "$1" $lpath; last=$?; shift;;
	*)	cppflags "$1" $ipath; last=$?; shift;;
	esac
done

printf '\n'
