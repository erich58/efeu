#!/bin/sh
# :*:grep for files of directories
# :de:grep für Dateien in Verzeichnisse
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

#	command usage

usage ()
{
	case ${LANG:=en} in
	de*)
		cat <<!
BEZEICHNUNG
	$0 - Dateien eines Verzeichnisses nach Mustern durchsuchen.

SYNTAX
	$0 [grep-Optionen] Muster Verzeichnis(se) [find-Optionen]

BESCHREIBUNG
	Das Kommando $0 stellt mit find eine Liste von Dateien
	zusammen, die anschließend mit grep nach dem Muster <Muster>
	durchsucht werden.

	Die grep-Optionen werden nicht geprüft, sondern nur durchgeschleußt.
	Die Optionen "-e" und "-f" müssen als letztes angegeben werden.
	Find wird automatisch mit den Flags "-type f" und "-print" aufgerufen.

SIEHE AUCH
	grep(1), find(1).
!
		;;
	*)
		cat <<!
NAME
	$0 - search pattern in files of directories

SYNOPSIS
	$0 [grep-options] pattern directories [find-options]

DESCRIPTION
	The command $0 search patterns with grep in a list
	of files collected by find.

	Find is called with the flags "-type f" and "-print".
	If supported, "-print" is replaced by "-print0".

SEE ALSO
	grep(1), find(1).
!
		;;
	esac
}

#	parse grep options

while [ $# -gt 0 ]
do
	case $1 in
	-[AB])	gopt="$gopt $1 $2"; shift 2;;
	-[-ef])	gopt="$gopt $1"; shift; break;;
	-*)	gopt="$gopt $1"; shift;;
	*)	break;;
	esac
done

#	check number of arguments

if [ $# -lt 2 ]; then
	usage
	exit 1
fi

#	get pattern

pat="$1"
shift

#	check if find supports -print0. This allows copying of files
#	with special characters like spaces

PFLAG="-print"
XFLAG=""

if
	find /dev/null -print0 > /dev/null 2>&1
then
	PFLAG="-print0"
	XFLAG="-0"
fi

#	start find

#echo find "$@" -type f $PFLAG "|" xargs $XFLAG grep $gopt $pat
exec find "$@" -type f $PFLAG | xargs $XFLAG grep $gopt "$pat"
