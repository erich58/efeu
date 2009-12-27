/*
Zufallszahlengenerator

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

#ifndef	EFEU_Rand48_h
#define	EFEU_Rand48_h	1

#include <EFEU/refdata.h>

/*	Hilfsfunktionen
*/

extern double drand48 (void);
extern long lrand48 (void);
extern void srand48 (long seedval);

extern double erand48 (unsigned short param[3]);
extern long nrand48 (unsigned short param[3]);


/*	Zufallszahlengenerator
*/

typedef struct {
	REFVAR;
	unsigned short param[3];
} Rand48_t;

extern reftype_t Rand48_reftype;

Rand48_t *NewRand48 (int sval);
void DelRand48 (Rand48_t *rd);
void SeedRand48 (Rand48_t *rd, int sval);
double Rand48 (Rand48_t *rd);
double NormalRand48 (Rand48_t *rd, int mode);
int Rand48Round (Rand48_t *rd, double val);


/*	Schnittstelle zu Interpreter
*/

void SetupRand48 (void);

#endif	/* EFEU_Rand48_h */
