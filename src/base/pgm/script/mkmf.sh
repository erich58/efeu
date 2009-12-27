#!/bin/sh
# :*:wraper for makefile-generators
# :de:Hülle für Makefile-Generatoren
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
# Version="$Id: mkmf.sh,v 1.5 2008-03-07 20:51:28 ef Exp $"
# n |
#	:*:print out the selected Makefile-generator without executing it.
#	:de:zeige den ausgewählten Makefile-Generator ohne ihn
#	aufzurufen.

usage ()
{
	efeuman -- $0 $1 || echo "usage: $0"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version="[$]Id:' $0; exit 0;;
esac

# $Description
# :*:The command |$!| is a wrapper for different Makefile-generators.
# It searches for typical configuration files and runs the according
# command.
# :de:Das Kommando |$!| ist ein Startprogramm für verschiedene
# Makefile-Generatoren. Es sucht nach typischen Konfigurationsdateien
# und startet das zugehörige Kommando.
#
# @arglist -i

# parse command args

run="exec"

while getopts n opt
do
	case $opt in
	n)	run="echo";;
	\?)	usage -?; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

if [ "$#" -ne 0 ]; then
	usage -?
	exit 1
fi

# $Description
# :de:
# Das Kommando |$!| prüft der Reihe nach die folgenden Regeln, bis der
#	Makefile-Generator identifiziert wurde:
# *	Falls die Datei |Config.make| existiert, wird |shmkmf| verwendet.
# *	Falls die Datei |Imakefile| exisitiert, wird zunächst nach einer Zeile,
# 	die mit |XCOMM| beginnt gesucht. Wenn vorhanden, wird |xmkmf| und
# 	ansonsten |ccmkmf| verwendet.
# *	Falls eine ausführbare Datei |configure| existiert, wird diese
#	ausgeführt.
# *	Falls ein |Makefile| existiert, wird mit |make Makefile| eine
#	Erneuerung des Makefiles probiert.
# *	Ansonsten wird eine Fehlermeldung ausgegeben.

if [ -f Config.make ]; then
	$run shmkmf
elif [ -f Imakefile ]; then
	if
		grep -s '^XCOMM' Imakefile
	then
		$run xmkmf
	else
		$run ccmkmf
	fi
elif [ -f configure -a -x configure ]; then
	$run ./configure
elif [ -f Makefile ]; then
	$run make Makefile
else
	case "$LANG" in
	de*) cat << EOF
Der Makefile-Generator konnte nicht identifiziert werden.
EOF
		;;
	*) cat << EOF
Could not determine the Makefile generator.
EOF
		;;
	esac

	exit 1
fi

# $Note
# :*:The command |ccmkmf| ist deprecated.
# :de:Das Kommando |ccmkmf| ist veraltet.
#
# $SeeAlso
# shmkmf(1), xmkmf(1), ccmkmf(1), autoconf(1).
