#!/bin/sh
# :*:file command with efeu extensions
# :de:file Kommando mit EFEU-Anpassung
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

top=`efeutop`
magic=$top/etc/magic

if [ -f $magic ]; then
	flags="-m $magic"
else
	flags=""
fi

tmp=${TMPDIR:-/tmp}/efile$$

for x
do
	case $x in
	*.gz)
		zcat -f $x | dd of=$tmp count=1 2>/dev/null
		file $flags $tmp | sed -e "s|$tmp|$x|"
		rm $tmp
		;;
	*)
		file $flags $x
		;;
	esac
done
