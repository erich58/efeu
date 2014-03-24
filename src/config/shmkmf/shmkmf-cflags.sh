#!/bin/sh
# :*:	determine c-flags for use of external libraries
# :de: 	Bestimmung von C-Flags zur Einbindung externer Bibliotheken
#
# $Copyright (C) 2008 Erich Frühstück
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

# $pconfig
# Version=":VN:"
#
# m::name |
#	:*:define macro name <name> with found flags.
#	:de:Makro <name> mit gefundenen Flags initialisieren.
# I::path |
#	:*:path to search header files (default: |/usr/include|).
#	:de:Suchpfad für Kopfdateien, Vorgabe: |/usr/include|.
# L::path |
#	:*:path to search libraries (default: |/lib:/usr/lib|).
#	:de:Suchpfad für Bibliotheken, Vorgabe: |/lib:/usr/lib|.
# a |
#	:*:try next search only if last search was successfull.
#	:de:Nächste Suche nur durchführen, wenn letzte Suche erfolgreich war.
# o |
#	:*:try next search only if last search failed.
#	:de:Nächste Suche nur durchführen, wenn letzte Suche fehlschlug.
# l:lib |
#	:*:search for libraray corresponding to the regular
#	expression |^lib(|<lib>|)\.(a||so)$$|. On asuccess, print out
#	the |-l|<name> flag for linking the library. If the library is
#	in an other place than |/lib| or |/usr/lib| an |-L|<dir> flag is
#	also generated.
#	:de:Suche nach der Bibliothek, die dem regulären
#	Ausdruck |^lib(|<lib>|)\.(a||so)$$| entspricht. Liefer bei
#	Erfolg das entsprechende |-l| flag und falls die Bibliothek
#	nicht in einem der beiden Verzeichnisse |/lib| oder |usr/lib|
#	liegt, optional auch ein eintsprechendes |-L|<dir> flag. 
# ::name |
#	:*:search diretory <dir> which contains header <name>, where <name> may
#	contain directory parts as in |X11/X.h|. If <dir> is different from
#	|/usr/include|, print out the option |-I|<dir>.
#	:de:Suche nach dem Verzeichnis <dir>, welches die Kopfdatei <name>
#	enthält, wobei <name> Pfadteile enthalten kann (z.B: |X11/X.h|)
#	Falls <dir> verschieden von |/usr/include| ist, wird ein Kompileflag
#	der Form |-I|<dir> erzeugt.

usage ()
{
	efeuman -- $0 $1 || cat << EOF
usage: $0 [-vpx] [-r arg] [-I dir] [-t top] config makefile
EOF
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

ipath=/usr/include
lpath=/lib:/usr/lib
out=
macname=CFLAGS

# Ausgabe erweitern

append ()
{
	if [ -n "$1" ]; then
		if [ -n "$out" ]; then
			out="$out $1"
		elif [ -n "$macname" ]; then
			out="$macname= $1"
		else
			out="$1"
		fi
	elif [ -n "$macname" ]; then
		out="$macname="
	fi
}

# Pfad erweitern

expand_path ()
{
	arg=`expr "$2" : '..\(.*\)'`

	if [ -z "$arg" ]; then
		echo ""
	elif [ -z "$1" ]; then
		printf "%s" "$arg"
	else
		printf "%s:%s" "$1" "$arg"
	fi
}

# Die Funktion spaltet eine Pfadvariable auf und fügt bei //
# alle möglichen Zwischenverzeichnisse mithilfe von find ein.
# Das Startverzeichnis für find enthält immer einen nachgestellen
# Schrägstrich, damit als Startverzeichnis ein symbolischer Link
# angegeben werden kann. Nicht alle von split_path gelieferten Verzeichnisse
# müssen existieren.

split_path ()
{(
	IFS=":"
	for d in $1
	do
		case "$d" in
		*?//?*)
			a=`expr $d : '\(.*/\)/'`
			b=`expr $d : '.*//\([^/]*\)'`
			c=`expr $d : '.*//[^/]*\(/.*\)'`
			find $a -type d -name "$b" -print 2>/dev/null |\
				sed -e "s|\$|$c|"
			;;
		*//)	find $d -type d -print 2>/dev/null;;
		*)	printf "%s\n" $d;;
		esac
	done
)}


# Include-Verzeichnis für Header bestimmen.

get_incdir ()
{
	while read dir
	do
		if [ -r $dir/$1 ]
		then
			case "$dir" in
			/usr/include//)	;;
			/usr/include/)	;;
			/usr/include)	;;
			*)		printf -- "-I%s\n" $dir;;
			esac

			return 0
		fi
	done

	return 1
}

# Link-Flags bestimmen

get_libname ()
{
	while read name
	do
		case "$name" in
		*.a)	lib=`expr "$name" : '.*/lib\(.*\).a$'`;;
		*.so)	lib=`expr "$name" : '.*/lib\(.*\).so$'`;;
		*)	continue;
		esac

		printf -- "-l%s\n" "$lib"
		return 0
	done

	return 1
}

get_lib ()
{
	name=`expr "$1" : '..\(.*\)'`

	while read dir
	do
		[ ! -d $dir ] && continue
		d=`cd $dir; pwd`

		if
			lib=`ls -d $d/lib$name.* 2>/dev/null | get_libname`
		then
			case "$d" in
			/lib)		;;
			/usr/lib)	;;
			*)		printf -- "-L%s " $d;;
			esac

			printf "%s\n" "$lib"
			return 0
		fi
	done
	return 1
}

last=0
stat=0

# Anweisungen abarbeiten

while [ -n "$1" ]
do
	case "$1" in
	-a)	shift; [ $last -eq 0 ] && shift;;
	-o)	shift; [ $last -ne 0 ] && shift;;
	-m*)	macname="`expr "$1" : '..\(.*\)'`"; shift;;
	-I*)	ipath=`expand_path "$ipath" "$1"`; shift;;
	-L*)	lpath=`expand_path "$lpath" "$1"`; shift;;
	-l*)	if
			x="`split_path "$lpath" | get_lib "$1"`"
		then
			append "$x"
			stat=1
			last=1
		else
			last=0
		fi
		shift
		;;
	-*)	usage "$1"; exit 1;;
	*)	if
			x="`split_path "$ipath" | get_incdir "$1"`"
		then
			append "$x"
			stat=1
			last=1
		else
			last=0
		fi
		shift
		;;
	esac
done

# Ergebnis ausgeben und Rückgabestatus bestimmen.
# Mehrfachschrägstriche in der Ausgabe werden reduziert.

if [ "$stat" -ne 0 ]; then
	printf "%s\n" "$out" | sed -e 's|/[/]*|/|g'
	exit 0
fi

exit 1
