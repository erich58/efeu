#!/bin/sh
# :*:copy directory
# :de:Verzeichnis kopieren
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
# Version=":VN:"
# :src|
#	:*:source directory
#	:de:Ausgangsverzeichnis
# :tg|
#	:*:target directory
#	:de:Zielverzeichnis
# ::"find-options"|
#	:*:any find option
#	:de:beliebige find - Option

usage ()
{
	efeuman -- $0 $1 || echo "usage: $0 src tg [find-options]"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

# $Description
# :*:
# The command |\$!| uses |find| and |cpio| to copy the directory tree
# <src> to <tg>|/|<src>.
# The target files gets the same file modification times then the original
# files. The access time of the source files is keept unchanged.
# Files are only overwritten, if the coresponding source files have a newer
# modifikation time.
# :de:
# Das Kommando |\$!| verwendet |find| und |cpio| zum Kopieren des
# Verzeichnisbaumes <src> nach <tg>|/|<src>.
# Attribute und Zugriffszeiten werden mitkopiert. Die Zugriffszeiten
# der Ausgangsdateien bleiben unverändert. Dateien werden nur überschrieben,
# wenn die zugehörige Ausgangsdatei eine neuere Modifikationszeit aufweist.

# $SeeAlso
# find(1), cpio(1), dircmp(1).

cpio_flags="-padmv"

if [ $# -lt 2 ]; then
	usage -?
	exit 1
fi

source="$1"
target="$2"
shift 2
echo find $source -depth "$@" -print
find $source -depth "$@" -print | cpio $cpio_flags "$target"
