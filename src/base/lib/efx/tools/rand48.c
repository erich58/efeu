/*
Implementation des Zufallszahlengenerator

$Copyright (C) 1994 Erich Frühstück
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


#include <EFEU/efutil.h>
#include <EFEU/rand48.h>

#define	X0	0x0000
#define	X1	0x0000
#define	X2	0x330E

#define	A0	0x0005
#define	A1	0xDEEC
#define	A2	0xE66D

#define	C	0x000B

#define	M	(1. / 0x10000)


static unsigned short std_param[3] = { 0x330E, 0, 0 };

static ALLOCTAB(Rand48_tab, 0, sizeof(Rand48_t));

static char *rident(Rand48_t *rand)
{
	return msprintf("0x%04x%04x%04x", rand->param[0],
		rand->param[1], rand->param[2]);
}

static Rand48_t *radmin(Rand48_t *tg, const Rand48_t *src)
{
	if	(tg)
	{
		del_data(&Rand48_tab, tg);
		return NULL;
	}
	else	return new_data(&Rand48_tab);
}


ADMINREFTYPE(Rand48_reftype, "Rand48", rident, radmin);

Rand48_t *NewRand48(int sval)
{
	Rand48_t *rand;

	rand = rd_create(&Rand48_reftype);
	SeedRand48(rand, sval);
	return rand;
}

void SeedRand48(Rand48_t *rand, int sval)
{
	register unsigned short *param;

	param = rand ? rand->param : std_param;

	param[0] = 0x330E;
	param[1] = ((unsigned) sval >> 16);
	param[2] = (0xFFFF & sval);
}


void DelRand48(Rand48_t *rand)
{
	rd_deref(rand);
}


double Rand48(Rand48_t *rand)
{
	register unsigned short *param;
	register unsigned a, b, c;

	param = rand ? rand->param : std_param;

	a = C + param[0] * A2;
	b = (a >> 16) + param[0] * A1;
	c = (b >> 16) + param[0] * A0;
	b = (b & 0xFFFF) + param[1] * A2;
	c = (c & 0xFFFF) + (b >> 16) + param[1] * A1 + param[2] * A2;
	param[2] = (ushort_t) c;
	param[1] = (ushort_t) b;
	param[0] = (ushort_t) a;
	return (M * (param[2] + M * (param[1] + M * param[0])));
}


int Rand48Round(Rand48_t *rand, double x)
{
	int val;
	double rval;

	val = (int) x;
	x -= val;
	rval = Rand48(rand);

	if	(x >= 0.)
	{
		if	(x > rval)	val++;
	}
	else	if	(-x > rval)	val--;

	return val;
}
