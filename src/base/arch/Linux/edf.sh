#!/bin/sh
# :*:disk usage, linux version
# :de:Festplatenverwendung, linux Version
#
# Copyright (C) 1996 Erich Frühstück
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

df | awk '
BEGIN {
	Blksize = 1024
#	MByte = 1048576
	MByte = 1000000
	CONST = Blksize / MByte
#	CONST = 1. / 1000.
	sb = 0
	su = 0
	sf = 0
}
/^File/ {
	printf "%-14s", "Filesystem"
	printf " %10s %13s %13s", "Gesamt MB", "Verbraucht", "Verfügbar"
	printf "  Mountpunkt\n"
	next
}
{
	printf "%-16s", $1
	b = $2 * CONST
	u = $3 * CONST
	f = $4 * CONST
#	f = b - u
	sb += b
	sf += f
	su += u
	printf " %8.2f %8.2f %3.0f%%", b, u, 100 * (u / b)
	printf " %8.2f %3.0f%%  %s\n", f, 100 * (f / b), $6
}
END {
	printf "%-16s", "Insgesamt"
	printf " %8.2f %8.2f %3.0f%%", sb, su, 100 * (su / sb)
	printf " %8.2f %3.0f%%\n", sf, 100 * (sf / sb)
}
'
