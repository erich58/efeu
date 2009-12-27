#!/bin/sh
# :*:transform data of ZIP-archive into gzip compressed file
# :de:ZIP Archiv in komprinierte Datei konvertieren
#
# $Copyright (C) 2002 Erich Frühstück
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
# a|
#	:*:get data in text mode (default)
#	:de:Daten im Textmodus auslesen (default)
# b|
#	:*:get data in binary mode
#	:de:Daten binär auslesen
# \#|
#	:*:compression mode (-1 fast, -9 best)
#	:de:Kompressionsmodus (-1 schnellste Kompression, -9 beste Kompression)
# :zip|
#	:*:zip archive
#	:de:ZIP-Archiv
# :out|
#	:*:output file
#	:de:Ausgabefile

usage ()
{
	efeuman -- $0 $1 || echo "usage: $0 [-ab#] zip out"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

# $Description
# :*:
# The command |\$!| extracts data from a zip-archive and creates
# a |gzip|-compressed file.
# :de:
# Das Kommando |\$!| holt die Daten aus einem zip-Archiv und generiert
# eine komprimierte Ausgabedatei.
#
# @arglist -i
#
# $SeeAlso
# gzip(1), unzip(1).

# parse command args

unzip_opt="-pa"
gzip_opt="-9"

while getopts ab0123456789 opt
do
	case $opt in
	a)	unzip_opt="-pa";;
	b)	unzip_opt="-p";;
	[0-9])	gzip_opt="-$opt";;
	\?)	usage -?; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

if [ $# -ne 2 ]; then
	usage -?
	exit 1
fi

exec unzip $unzip_opt $1 | gzip -n $gzip_opt > $2
