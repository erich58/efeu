/*
Zufallszahlengeneratoren

$Header <EFEU/$1>

$Copyright (C) 1998 Erich Frühstück
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

#ifndef	EFEU_Random_h
#define	EFEU_Random_h	1

#include <EFEU/refdata.h>
#include <EFEU/io.h>

#if	_AIX

extern int random (void);
extern void srandom (unsigned int sval);
extern char *initstate (unsigned sval, char *state, int dim);
extern char *setstate (char *state);

#endif

#define	DRAND_KOEF	(1. / 2147483648.)	/* 2^-31 */

/*	Zufallsgeneratortype
*/

typedef struct RandomType_s RandomType_t;

struct RandomType_s {
	char *name;
	char *desc;
	void (*ident) (io_t *io, void *data);
	void *(*init) (unsigned int sval);
	void *(*copy) (const void *data);
	void (*clean) (void *data);
	void (*seed) (void *data, unsigned int sval);
	double (*rand) (void *data);
};

extern RandomType_t RandomType_48;
extern RandomType_t RandomType_x48;
extern RandomType_t RandomType_std;
extern RandomType_t RandomType_old;
extern RandomType_t RandomType_r0;
extern RandomType_t RandomType_r1;
extern RandomType_t RandomType_r2;
extern RandomType_t RandomType_r3;
extern RandomType_t RandomType_r4;

void AddRandomType (RandomType_t *type);
void ListRandomType (io_t *io);

RandomType_t *GetRandomType (const char *str);


/*	Zufallsgenerator
*/

typedef struct {
	REFVAR;
	RandomType_t *type;
	void *data;
} Random_t;

extern reftype_t Random_reftype;

Random_t *Random (unsigned int sval, RandomType_t *type);
Random_t *str2Random (const char *str);
Random_t *CopyRandom (Random_t *rd);

void SeedRandom (Random_t *rd, unsigned int sval);
double UniformRandom (Random_t *rd);

/*	Zufallsfunktionen
*/

double LinearRandom (Random_t *rd, double z);
double NormalRandom (Random_t *rd);
int PoissonRandom (Random_t *rd, double mw);
int RoundRandom (Random_t *rd, double val);
size_t RandomIndex (Random_t *rd, size_t dim);

/*	Initialisierung für esh-Interpreter
*/

void SetupRandom (void);

#endif	/* EFEU_Random_h */
