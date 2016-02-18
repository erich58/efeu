#!/bin/sh
# :*:sanity check for compiling EFEU
# :de:Vorraussetzungsprüfung für die EFEU Kompilation
#
# $Copyright (C) 2008 Erich Frühstück
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

TMP=`mktemp -d`
trap "rm -rvf $TMP;" 0
trap "exit 1" 1 2 3 15
readonly TMP

check_stat ()
{
	stat=$1
	shift

	printf "%s " "$*"

	case "$stat" in
	0)	printf "SUCCESS\n";;
	*)	printf "FAILURE\n"; exit 1;;
	esac
}

cd $TMP
cat > hello.c << !
#include <stdio.h>

int main (int argc, char **argv)
{
	printf("Hello World!\n");
	return 0;
}
!

make hello > /dev/null
check_stat $? "Testing compiler ....................... "

rm -rf $TMP
