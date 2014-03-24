#!/bin/sh
# :*:check efeu version
# :de:EFEU-Version überprüfen
#
# $Copyright (C) 2011 Erich Frühstück
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
# ::"vn"|
#	:*:mimum version desired
#	:de:Mindestversion

usage ()
{
	efeuman -- $0 $1 || echo "usage: $0 src tg [find-options]"
}

case "$1" in
-\?|--help*)	usage $1; exit 0;;
--version)	efeuman -- $0 $1 || grep 'Version[=]' $0; exit 0;;
esac

Version=":VN:"
actual=0
desired=0

bad_version ()
{
	echo "Version to old ($actual < $desired)." >&2
	exit 1
}

check ()
{
	while read num
	do
		[ $# -gt 0 ] || bad_version
		[ "$num" -lt "$1" ] && exit 0
		[ "$num" -gt "$1" ] && bad_version
		shift
	done
}

case $# in
0)
	echo $Version
	;;
*)
	actual=`expr "$Version" : '[^0-9]*\([-0-9.]*\)' | sed -e 's/[-.]/ /g'`
	desired=`echo "$*" | sed -e 's/[-.]/ /g'`
	printf "%d\n" $desired | check $actual
	;;
esac

