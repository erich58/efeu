#!/bin/sh
# :*:	show the description of a config file
# :de: 	Beschreibung einer KOnfigurationsdatei ausgeben
#
# $Copyright (C) 2016 Erich Frühstück
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
# :label |
#	:*:description label
#	:de:Beschreibungslabel
# :cfg |
#	:*:name of the config file
#	:de:Name der Konfigurationsdatei

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

if	[ $# -ne 2 ]
then	usage -?; exit 1
fi

: ${LANG:=en}
language=`expr "$LANG" : "\([a-z][a-z]\).*"`
: ${language:=en}

awk -v key=":$language:" -v label="$1" '
! /^#[ \t]/ { exit }
{
	if	(match($0, ":[*]:[ \t]*"))
	{
		default = substr($0, RSTART + RLENGTH);
	}
	else if	(match($0, ":[^:]*:"))
	{
		if	(key == substr($0, RSTART, RLENGTH))
		{
			if	(match($0, ":[^:]*:[ \t]*"))
			{
				desc = substr($0, RSTART + RLENGTH);
			}
		}
	}
	else if	(match($0, "^#[ \t]*"))
	{
		default = substr($0, RSTART + RLENGTH);
	}
}
END {
	printf("%s", label);

	if	(length(label) > 7)
		printf("\n");

	if	(length(desc))
		printf("\t%s\n", desc);
	else if	(length(default))
		printf("\t%s\n", default);
	else
		printf("\n");
}
' $2
