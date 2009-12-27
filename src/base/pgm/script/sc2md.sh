#!/bin/sh
# :*:transforms sc representation of data cube to binary form
# :de:Konvertiert sc Darstellung einer Datenmatrix in Binärformat
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

usage="usage: $0 [-r] input output"

case ${LANG:=en} in
de*)
	usage="Aufruf: $0 [-r] ein aus"
	;;
esac


flags=

if
	test "X$1" = "X-r"
then
	flags="$flags -r";
	shift;
fi

if
	test $# -ne 2
then
	echo "$usage"
	exit 1
fi

tmp=${TMPDIR:-/tmp}/sc2md$$

sc $1 >/dev/null <<!
T$tmp
q
!

if 
	test -f $tmp
then
	mdread $flags $tmp $2
	rm -f $tmp
fi
