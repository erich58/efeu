#!/bin/sh
# :*:search pattern in files of directories
# :de:Dateien eines Verzeichnisses nach Mustern durchsuchen.
#
# $Copyright (C) 2001 Erich Frühstück
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
# ::"grep-options"|
#	:*:any grep option
#	:de:beliebige grep - Option
# ::"<=<pat>~\|~|-e|~<pat>~\|~|-f|~<file>=>"|
#	:*:search pattern or file
#	:de:Suchmuster oder Datei
# :dir|
#	:*:directories
#	:de:Verzeichnisse
# ::"find-options"|
#	:*:any find option
#	:de:beliebige find - Option

usage ()
{
	efeuman -- $0 $1 || cat << EOF
usage: $0 [grep-options] pattern
	directories [find-options]
EOF
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
esac

# $Description
# :*:
# The command |\$!| search patterns with |grep| in a list
# of files collected by |find|. To avoid environment space overflow
# |xargs| is used to execute |grep|.
# The grep options are passed through and not checked.
# :de:
# Das Kommando |\$!| stellt mit |find| eine Liste von Dateien
# zusammen, die anschließend mit |grep| nach dem Muster <Muster>
# durchsucht werden. Damit der Umgebungspeicher nicht überläuft, wird
# |xargs| zum Aufruf von |grep| verwendet.
# Die grep-Optionen werden nicht geprüft, sondern nur durchgeschleußt.
#
# :*:
# Find is called with the flags |-type f| and |-print|.
# If supported, |-print| is replaced by |-print0|.
# This allows to handle files with special characters like spaces.
# :de:
# Find wird automatisch mit den Flags |-type f| und |-print| aufgerufen.
# Falls find |-print0| ünterstützt, wird diese Option anstelle von |-print|
# verwendet. Daduch können auch Dateien mit Leerzeichen und anderen
# Sonderzeichen im Namen verarbeitet werden.

# $SeeAlso
# grep(1), find(1), xargs(1).

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
	usage -?
	exit 1
fi

#	get pattern

pat="$1"
shift

#	check if find supports -print0. This allows to handle files
#	with special characters like spaces.

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
