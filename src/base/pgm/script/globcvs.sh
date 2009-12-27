#!/bin/sh
# :*:call cvs for a list of repositories
# :de:cvs - Aufruf für mehrere Arbeitskopien
#
# Copyright (C) 2001 Erich Frühstück
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

#	command usage

usage ()
{
	case ${LANG:=en} in
	de*)
		cat <<!
Aufruf: $0 --ls | cvs Parameter

Das Kommando `basename $0` führt ein cvs-Kommando für alle
Arbeitskopien unterhalb der durch die Umgebungsvariable GLOBCVS
definierten Verzeichnisliste durch. Falls GLOBCVS nicht definiert
ist, wird \$EFEUTOP/src genommen.

Die aktuelle Setzung von GLOBCVS ist "$GLOBCVS".

Die Option --ls listet alle Verzeichnisse auf, für die ein eigenes
cvs-Kommando abgesetzt wird.

!
		;;
	*)
		cat <<!
usage: $0 --ls | cvs parameter

The command `basename $0` calls cvs for all working directories speified
under the list of directories, specified by de Environment GLOBCVS.
If GLOBCVS is not defined, \$EFEUTOP/src is used, if EFEUTOP is not
defined, \$HOME is used.

The current setting of GLOBCVS is "$GLOBCVS".

The option --ls lists all directories, where a separate call of cvs is
executed.

!
		;;
	esac
}  


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
