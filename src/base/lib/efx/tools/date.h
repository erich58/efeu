/*
Datumskonvertierungen

$Header <EFEU/$1>

$Copyright (C) 1996 Erich Frühstück
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

#ifndef	EFEU_DATE_H
#define	EFEU_DATE_H	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>

typedef struct {
	int day;
	int month;
	int year;
	int wday;	/* Wochentagnummer, 0 = Sonntag)
	int dofy;	/* Tag des Jahres */
	int wofy;	/* Woche des Jahres */
} Date_t;

unsigned Today1900 (void);
unsigned Date1900 (int tag, int monat, int jahr);
Date_t *KonvDate1900 (unsigned date, Date_t *buf);

PrintDate (io_t *io, const char *fmt, Date_t *date);
char *Date2str (io_t *io, const char *fmt, Date_t *date);

#endif	/* EFEU_DATE_H */
