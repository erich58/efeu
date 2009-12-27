#!/bin/sh
# :*:edit binary file using mkdump and vi
# :de:binäre Dateien mit mkdump und vi editieren
#
# $Copyright (C) 1991 Erich Frühstück
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
# Version="$Id: binedit.sh,v 1.7 2004-10-25 05:46:27 ef Exp $"
# :file |
#	:*:file to edit
#	:de:Datei zum editieren

usage ()
{
	efeuman -- $0 $1 || echo "usage: $0 file"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version="[$]Id:'; exit 0;;
esac

# $Description
# :en:The command transforms the binary file <file> into a temporary
# ascii representation and starts |vi| to edit.\par
# The following options and arguments are accepted:
# :de:Das Programm formt die Binärdatei <file> in eine temporäre
# Datei um und startet den vi zum editieren.\par
# Die folgenden Optionen und Argumente werden akzeptiert:
#
# @arglist
#
# $SeeAlso
# mkdump(1), undump(1).

#	message formats

msg1="usage: $0 file\n"
msg2="$0: file \"%s\" not readable\n"
msg3="$0: file \"%s\" not writeable\n"
msg4="
The command transforms the binary file \"%s\" into a temporary
ascii representation and starts vi to edit.\n
"
msg5="continue ? j\b"
msg6="save changes ? j\b"

case ${LANG:=en} in
de*)
	msg1="Aufruf: $0 Filename\n"
	msg2="$0: File \"%s\" nicht lesbar\n"
	msg3="$0: File \"%s\" nicht beschreibbar\n"
	msg4="
Das Programm formt die Binärdatei \"%s\" in eine temporäre
Datei um und startet den vi zum editieren.\n
"
	msg5="Fortfahren ? j\b"
	msg6="Änderungen sichern ? j\b"
	;;
esac

#	check command args

if
	test $# -ne 1
then
	usage -?
	exit 1
elif
	test ! -r $1
then
	printf "$msg2" "$1" 1>&2
	exit 1
elif
	test ! -w $1
then
	printf "$msg3" "$1" 1>&2
	exit 1
fi

#	edit file

printf "$msg4" "$1"

if
	printf "$msg5" 1>&2
	read x
	test "$x" != "n"
then
	tmp=${TMPDIR:-/tmp}/xvi$$
	mkdump $1 $tmp
	vi $tmp

	if
		printf "$msg6" 1>&2
		read x
		test "$x" != "n"
	then
		undump $tmp $1
	fi

	rm $tmp
fi
