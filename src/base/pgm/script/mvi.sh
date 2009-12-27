#!/bin/sh
# :*:edit data cube using vi
# :de:Datenmatrix mit vi editieren
#
# $Copyright (C) 1997 Erich Frühstück
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

: ${LANG:=en}

# $pconfig
# Version="$Id: mvi.sh,v 1.7 2007-08-19 04:22:10 ef Exp $"
# file |
#	name of file
# :mdprint options |
#	mdprint options

usage ()
{
	efeuman -- $0 $1 || echo "usage: $0 file [mdprint options]"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version="[$]Id:' $0; exit 0;;
esac

# $Description
# :*:
# The command allows to edit the data cube <file> with vi.
# Depending on the mdprint options, the data cube may be changed.
# You should edit carefully, a incorrect syntax may result in
# data loss.
# :de:
# Das Programm erlaubt die Manipulation der Datenamtrix <file>
# mit dem Editor vi. Je nach Art der mdprint-Optionen, ist eine
# Änderung der Datenmatrix möglich.  Die Änderungen an der Datenmatrix
# sollten mit Vorsicht durchgeführt werden, ein Fehler in der Syntax kann
# zu Datenverlusten führen.

# $SeeAlso
# mdprint(1), mdread(1), msc(1).

#	message formats

msg1="usage: $0 file [mdprint options]\n"
msg2="$0: file \"%s\" not readable\n"
msg3="$0: file \"%s\" not writeable\n"
msg4="
The command allows to edit the data cube \"%s\" with vi.
Depending on the mdprint options, the data cube may be changed.
You should edit carefully, a incorrect syntax may result in
data loss.\n
"
msg5="continue ? j\b"
msg6="save changes ? j\b"

case ${LANG:=en} in
de*)
	msg1="Aufruf: $0 Filename [mdprint Optionen]\n"
	msg2="$0: File \"%s\" nicht lesbar\n"
	msg3="$0: File \"%s\" nicht beschreibbar\n"
	msg4="
Das Programm erlaubt die Manipulation der Datenamtrix \"$name\"
mit dem Editor vi. Je nach Art der mdprint-Optionen, ist eine
Änderung der Datenmatrix möglich. Die Änderungen an der Datenmatrix
sollten mit Vorsicht durchgeführt werden, ein Fehler in der Syntax kann
zu Datenverlusten führen.\n
"
	msg5="Fortfahren ? j\b"
	msg6="Änderungen sichern ? j\b"
	;;
esac

#	check command args

if
	test $# -lt 1
then
	usage -?
	exit 1
fi

name=$1
shift

if
	test ! -r $name
then
	printf "$msg2" "$name" 1>&2
	exit 1
elif
	test ! -w $name
then
	printf "$msg3" "$name" 1>&2
	exit 1
fi

#	edit file

tmp=${TMPDIR:-/tmp}/mvi$$
printf "$msg4" "$name"

if
	echo mdprint $name $tmp "$@"
	printf "$msg5" 1>&2
	read x
	test "$x" != "n"
then
	mdprint $name $tmp "$@"
	vi $tmp

	if
		printf "$msg6" 1>&2
		read x
		test "$x" != "n"
	then
		mdread $tmp $name
	fi

	rm $tmp
fi
