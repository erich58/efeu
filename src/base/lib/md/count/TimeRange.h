/*
Datumsbereich für Auszählungen

$Header <EFEU/$1>

$Copyright (C) 1998, 2007 Erich Frühstück
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

#ifndef	EFEU_TimeRange_h
#define	EFEU_TimeRange_h	1

#include <EFEU/vecbuf.h>
#include <EFEU/mstring.h>
#include <EFEU/io.h>
#include <EFEU/calendar.h>
#include <EFEU/object.h>

typedef struct {
	char *label;	/* Bereichskennung */
	unsigned ug;	/* Intervalluntergrenze */
	unsigned og;	/* Intervallobergrenze */
	unsigned dat;	/* Bezugsdatum für Stammdaten */
	unsigned jahr;	/* Bezugsjahr für Stammdaten */
	unsigned tage;	/* Intervallbreite */
} TimeRange;

int IOTimeRange (VecBuf *buf, IO *io);
int strTimeRange (VecBuf *buf, const char *def);
void TimeRange_sync (TimeRange *rng);
int PrintTimeRange (IO *io, TimeRange *rng, size_t n);
extern char *TimeRange_fmt;
extern EfiType Type_trange;

#endif	/* EFEU/TimeRange.h */
