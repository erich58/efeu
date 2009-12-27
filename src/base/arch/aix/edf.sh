#!/bin/sh
# :*:disk usage, aix version
# :de:Festplatenverwendung, AIX Version
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

tmp=/var/tmp/df$$

#	Filesysteme

VGList()
{
	for x in `lsvg`
	do
		lsvgfs $x | sed -e "s/$/	$x/"
	done
}

DFList()
{
	VGList | sort > $tmp.1

	df | awk '
/^File/ { next }
{ printf "%s\t%s\t%s\t%s\n", $7, $1, $2, $3 }
' | sort > $tmp.2
	join $tmp.1 $tmp.2
	rm -f $tmp.1 $tmp.2
}

DFShow()
{
	awk '
BEGIN {
	CONST = 1024 / 1000000
	sb = 0
	su = 0
	sf = 0
	gsb = 0
	gsu = 0
	gsf = 0
	grp = ""
	printf "\n%-14s %10s ", "Filesystem", "Gesamt MB"
	printf "%13s ", "Verbraucht"
	printf "%13s ", "Verfügbar"
	printf " %-8s %s\n", "Gruppe", "Mountpunkt"
}
{
	if	(grp != $2)
	{
		if	(gsb)
		{
			printf "%-16s %8.2f ", "*Gruppe", gsb
			printf "%8.2f %3.0f%% ", gsu, 100 * (gsu / gsb)
			printf "%8.2f %3.0f%% ", gsf, 100 * (gsf / gsb)
			printf " %-8s %s\n", grp, ""
		}

		gsb = 0
		gsu = 0
		gsf = 0
	}

	grp = $2
	b = $4 * CONST
	f = $5 * CONST
	u = b - f
	sb += b
	sf += f
	su += u
	gsb += b
	gsf += f
	gsu += u
	printf "%-16s %8.2f ", $3, b
	printf "%8.2f %3.0f%% ", u, 100 * (u / b)
	printf "%8.2f %3.0f%% ", f, 100 * (f / b)
	printf " %-8s %s\n", $2, $1
}
END {
	if	(gsb != sb)
	{
		printf "%-16s %8.2f ", "*Gruppe", gsb
		printf "%8.2f %3.0f%% ", gsu, 100 * (gsu / gsb)
		printf "%8.2f %3.0f%% ", gsf, 100 * (gsf / gsb)
		printf " %-8s %s\n", grp, ""
	}

	printf "%-16s %8.2f ", "*Insgesamt", sb
	printf "%8.2f %3.0f%% ", su, 100 * (su / sb)
	printf "%8.2f %3.0f%% ", sf, 100 * (sf / sb)
	printf " %-8s %s\n", "*", ""
}
'
}

DFList | sort -k 2,2 -k 1,1 | DFShow

#	Physikalischen Speicher auflisten

PVList()
{
	for x in `lspv | cut -d' ' -f1`
	do
		lspv $x | awk '
/PHYSICAL/ { name = $3; vg = $6 }
/PP SIZE/ { size = $3 + 0 }
/TOTAL PPs/ { total = $3 * size }
/USED PPs/ { used = $3 * size }
/FREE PPs/ { free = $3 * size }
END {
	printf "%s\t%s\t%s\t%s\t%s\n", name, vg, total, used, free
}
'
	done
}

PVShow()
{
	awk '
BEGIN {
	st = 0
	su = 0
	sf = 0
	SIZE = 1.048576
	printf "\n%-14s", "Festplatte"
	printf " %10s %13s %13s", "Gesamt MB", "Verbraucht", "Verfügbar"
	printf "  Gruppe\n"
}
{
	t = $3 * SIZE
	u = $4 * SIZE
	f = $5 * SIZE
	st += t
	su += u
	sf += f
	printf "%-16s", $1
	printf " %8.2f %8.2f %3.0f%%", t, u, 100 * (u / t)
	printf " %8.2f %3.0f%%  %s\n", f, 100 * (f / t), $2
}
END {
	printf "%-16s", "Insgesamt"
	printf " %8.2f %8.2f %3.0f%%", st, su, 100 * (su / st)
	printf " %8.2f %3.0f%%\n", sf, 100 * (sf / st)
}
'
}

PVList | PVShow
