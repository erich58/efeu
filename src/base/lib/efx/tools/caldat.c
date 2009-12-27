/*
Test auf kalendarisches Datum

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


/*	Zahl der Tage eines Monats
*/

static int tage[13] = {
	29,	/* Februar, wenn Schaltjahr */
	31,	/* Jänner */
	28,	/* Februar */
	31,	/* März */
	30,	/* April */
	31,	/* Mai */
	30,	/* Juni */
	31,	/* Juli */
	31,	/* August */
	30,	/* September */
	31,	/* Oktober */
	30,	/* November */
	31,	/* Dezember */
};


/*	Die Funktion liefert 0 bei korrektem Datum, 1 bei falscher
	Tagesangabe und 2 bei falscher Monatsangabe. Der
	Jahrhundertausgleich (entfällt im Jahr 2000!) ist
	nicht berücksichtigt.
*/

int TestDat(OldDatum_t dat)
{
	int x;

	if	(dat.monat < 1 || dat.monat > 12)	return 1;

	x = (dat.monat == 2 && dat.jahr % 4 == 0) ? 0 : dat.monat;

	if	(dat.tag < 1 || dat.tag > tage[x])	return 2;

	return 0;
}
