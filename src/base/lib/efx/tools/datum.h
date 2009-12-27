/*
Datumskonvertierungen

$Header <EFEU/$1>

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

#ifndef	EFEU_DATUM_H
#define	EFEU_DATUM_H	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>

typedef struct {
#if	REVBYTEORDER
	ushort_t jahr;
	uchar_t monat;
	uchar_t tag;
#else
	uchar_t tag;
	uchar_t monat;
	ushort_t jahr;
#endif
} OldDatum_t;

unsigned Dat2int (OldDatum_t dat);
OldDatum_t int2Dat (unsigned dat);
OldDatum_t Datum (int tag, int monat, int jahr);

int Dat_WTAG (OldDatum_t dat);
int Dat_TDJ (OldDatum_t dat);
int Dat_WDJ (OldDatum_t dat, int base);

int PrintDatum (io_t *io, const char *fmt, OldDatum_t dat);

OldDatum_t str2Dat (const char *str);
char *Dat2str (OldDatum_t dat);
int TestDat (OldDatum_t dat);

char *KonvDatum(const char *fmt, int datum);

extern char *MoName[13];
extern char *WoName[7];

#endif	/* EFEU_DATUM_H */
