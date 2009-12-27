#!/bin/sh
# :*:compose file from single parts
# :de:Datei aus Einzelteilen zusammenstellen
#
# $Copyright (C) 2007 Erich Frühstück
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
# Version="$Id: compose-file.sh,v 1.3 2007-08-19 04:22:10 ef Exp $"
# r|
#	:*:create makefile rules
#	:de:Make-Regeln generieren
# :base|
#	:*:basic file
#	:de:Basisdateien
# ::out|
#	:*:output file
#	:de:Ausgabedatei

usage ()
{
	efeuman -- $0 $1 || printf "usage: $0 [--help] [-r] base [out]\n"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version="[$]Id:' $0; exit 0;;
esac

# $Description
# :*:The command |\$!| reads the file <base> und replaces lines of
# the form:\br
# |@include foo|\br
# by the contents of file <foo>. Path names must be given relative to <base>.
# If option |-r| is set, the commands writes make rules to the standard
# output.
# :de:Das Kommando |\$!| liest die Datei <base> und ersetzt Zeilen der
# Form:\br
# |@include foo|\br
# durch den Inhalt der jeweiligen Datei <foo>. Die Pfade müssen
# relativ zur Basisdatei angegeben werden.
# Falls Option |-r| gesetzt wurde, werden Generierungsregeln für |make|
# zur Standardausgabe geschrieben.

mkrule=0

while getopts "r" opt
do
	case $opt in
	r)	mkrule=1;;
	\?)	usage -?; exit 1;;
	esac
done

shift `expr $OPTIND - 1`

oname=
output=

case "$#" in
1)
	if [ "$mkrule" -ne 0 ]; then
		printf "$0: option -r needs output name.\n"
		exit 1
	fi
	;;
2)
	oname="$2"
	output="> $2"
	;;
*)
	usage -?
	exit 1
	;;
esac

d=`dirname $1`

if [ "$mkrule" -ne 0 ]; then
	cat << !

all:: $oname

clean::
	rm -f $oname

$oname: $1
	$0 $1 $oname

Makefile: $1

!
	awk '/^@include/ { printf "%s: %s/%s\n", \
		"'"$oname"'", "'"$d"'", $2 }' $1
else
	awk '
/^@include/ { system("cat '"$d"'/" $2); next }
{ print }
' $1 | eval cat $output
fi
