#!/bin/sh
# :*:file command with efeu extensions
# :de:file Kommando mit EFEU-Anpassung
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
# Version="$Id: efile.sh,v 1.3 2004-11-06 06:28:04 ef Exp $"
# :file(s)|
#	:*:list of files to determine file types
#	:de:Liste der Dateien zur Bestimmung des Dateitypes

usage ()
{
	efeuman -- $0 $1 || echo "usage: $0 file(s)"
}

case "$1" in
""|-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version="[$]Id:'; exit 0;;
esac

# $Description
# :*:|$!| implements the |file| command in a efeu specific environment.
# It uses <EFEUTOP>|/etc/magic| as magic file. For files with suffix |.gz|
# the first Block is uncompressed and analysed by |file|.
# :de:|$!| implementiert das Kommando |file| mit efeu-spezifischen
# Erweiterungen. So wird als magic-File <EFEUTOP>|/etc/magic| verwendet.
# Bei Dateien mit Filezusatz |.gz| wird der erste Block der Datei entpackt
# und von |file| ausgewertet.
#
# @arglist -i

# $SeeAlso
# efeu-magic(1), file(1), magic(5).

top=`efeutop`
magic=$top/etc/magic

if [ -f $magic ]; then
	flags="-m $magic"
else
	flags=""
fi

tmp=${TMPDIR:-/tmp}/efile$$

for x
do
	case $x in
	*.gz)
		zcat -f $x | dd of=$tmp count=1 2>/dev/null
		file $flags $tmp | sed -e "s|$tmp|$x|"
		rm $tmp
		;;
	*)
		file $flags $x
		;;
	esac
done
