/*
Datumsangabe aus String gewinnen

$Copyright (C) 1995 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/datum.h>

OldDatum_t Datum(int tag, int monat, int jahr)
{
	OldDatum_t x;
	x.jahr = (jahr > 100 ? jahr : jahr + 1900);
	x.monat = monat;
	x.tag = tag;
	return x;
}

OldDatum_t str2Dat(const char *str)
{
	OldDatum_t x;
	int t, m, j;

	t = m = j = 0;
	sscanf((char *) str, "%d.%d.%d", &t, &m, &j);
	x.jahr = (j > 100 ? j : j + 1900);
	x.monat = m;
	x.tag = t;
	return x;
}

char *Dat2str(OldDatum_t dat)
{
	static char buf[12];

	sprintf(buf, "%2d.%2d.%4d", dat.tag, dat.monat, dat.jahr);
	return (char *) &buf;
}
