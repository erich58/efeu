#!/bin/sh
# :*:Ersetze den CVS-Root Namen in einer Arbeitskopie
# :de:change repository name in a working directory
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
# ::root |
#	:*:new name of repository
#	:de:Neuer Name des Archivs

usage ()
{
	efeuman -v "glob=$GLOBCVS" -- $0 $1 || \
		echo "usage: $0 dir [root]"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

# $Description
# :*:The command |\$!| replaces the repository name in |Root|
# against the new name <root> in all |CVS| subdirectories of the current
# directory. If no repository name ist given, the old name is listet.
# If more than one name ist listed, not all working directories
# belongs to the same repository.
# :de:Das Kommando |\$!| ersetzt den Archivnamen in |Root|
# durch den neuen Namen <root> in allen |CVS|-Unterverzeichnissen der 
# aktuellen Bibliothek. Falls keine Name angegeben wurde,
# wird der aktuelle Name angeszeigt.
# Wird mehr als ein Name angezeigt, gehören nicht alle Arbeitskopien
# zum gleichen Archiv.

# $SeeAlso
# cvs(1).

if	[ $# -eq 0 ]; then
	find . -name CVS | sed -e 's|$|/Root|' | xargs cat | uniq | sort -u
elif	[ $# -eq 1 ]; then
	find . -name CVS | while
	read	name
	do
		rm -f $name/Root
		echo "$1" > $name/Root
	done
else
	usage
fi

