#!/bin/sh
# :*:	list the next level of config files in a directory
# :de: 	Auflisten der nächsten Ebene von Konfigurationsdateien.
#
# $Copyright (C) 2009 Erich Frühstück
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
# :dir |
#	:*:name of the main directory
#	:de:Name des Hauptverzeichnisses

usage ()
{
	efeuman -- $0 $1 || cat << EOF
usage: $0 dir
EOF
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

if	[ $# -ne 1 ]
then	usage -?; exit 1
fi

cd "$1" || exit 0

find . -name Config.make | awk -F/ '
BEGIN { OFS = "/" }
NF > 1 { $0 = substr($0, 3); NF--; print }
' | sort | awk -F/ '
BEGIN { last = 0 }
NF < last || $last != key { last = NF; key = $NF; print }
'
