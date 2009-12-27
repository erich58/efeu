#!/bin/sh
# :*:call cvs for a list of repositories
# :de:cvs - Aufruf für mehrere Arbeitskopien
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

: ${EFEUTOP:=~}
: ${GLOBCVS:=$EFEUTOP/src}

# $pconfig
# Version="$Id: globcvs.sh,v 1.8 2004-10-25 05:46:27 ef Exp $"
# *"|--ls| <=\\|=> cvs parameter" |
#	:*:any cvs option or argument
#	:de:beliegige cvs Option oder Argument

usage ()
{
	efeuman -v "glob=$GLOBCVS" -- $0 $1 || \
		echo "usage: $0 --ls | cvs parameter"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version="[$]Id:'; exit 0;;
esac

# $Description
# :*:The command |\$!| calls cvs for all working directories specified
# under the list of directories, specified by de Environment GLOBCVS.
# If |GLOBCVS| is not defined, |\$EFEUTOP/src| is used, if |EFEUTOP| is
# not defined, |\$HOME/src| is used.
# :de:Das Kommando |\$!| führt ein cvs-Kommando für alle
# Arbeitskopien unterhalb der durch die Umgebungsvariable |GLOBCVS|
# definierten Verzeichnisliste durch. Falls |GLOBCVS| nicht definiert
# ist, wird |\$EFEUTOP/src| genommen, falls auch |EFEUTOP| nicht definiert
# ist, wird |\$HOME/src| verwendet.
#
# :*:The current setting of |GLOBCVS| is |@{glob}|.
# :de:Die aktuelle Setzung von |GLOBCVS| ist |@{glob}|.
#
# :*:The option |--ls| lists all directories, where a separate call of
# cvs is executed.
# :de:Die Option |--ls| listet alle Verzeichnisse auf, für die ein eigenes
# cvs-Kommando abgesetzt wird.

# $SeeAlso
# cvs(1).

if	[ $# -lt 1 ]; then
	usage
	exit 1
elif	[ "A$1" = "A--ls" ]; then
	cmd=":"
else
	cmd="cvs"
fi

for x in `echo $GLOBCVS | sed -e 's/:/ /g'`
do
	(cd $x

	if [ -d CVS ]; then
		echo `pwd`:
		$cmd "$@"
	else
		for y in `ls`
		do
			if [ -d $y ]; then
				GLOBCVS="$y" $0 "$@"
			fi
		done
	fi
	)
done
