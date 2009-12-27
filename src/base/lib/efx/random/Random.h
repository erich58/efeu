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

/*	Zufallsgeneratortype
*/

typedef struct RandomTypeStruct RandomType;

struct RandomTypeStruct {
	char *name;
	char *desc;
	void (*ident) (IO *io, void *data);
	void *(*init) (unsigned int sval);
	void *(*copy) (const void *data);
	void (*clean) (void *data);
	void (*seed) (void *data, unsigned int sval);
	double (*rand) (void *data);
};

extern RandomType RandomType_d48;
extern RandomType RandomType_std;
extern RandomType RandomType_old;
extern RandomType RandomType_r0;
extern RandomType RandomType_r1;
extern RandomType RandomType_r2;
extern RandomType RandomType_r3;
extern RandomType RandomType_r4;

void AddRandomType (RandomType *type);
void ListRandomType (IO *io);

RandomType *GetRandomType (const char *str);


/*	Zufallsgenerator
*/

typedef struct {
	REFVAR;
	RandomType *type;
	void *data;
} Random;

Random *NewRandom (RandomType *type, unsigned int sval);
Random *str2Random (const char *str, char **ptr);
Random *CopyRandom (Random *rd);

void SeedRandom (Random *rd, unsigned int sval);
double UniformRandom (Random *rd);

/*	Zufallsfunktionen
*/

double LinearRandom (Random *rd, double z);
double NormalRandom (Random *rd);
int PoissonRandom (Random *rd, double mw);
int RandomRound (Random *rd, double val);
size_t RandomIndex (Random *rd, size_t dim);
size_t RandomChoice (Random *rd, void *base, size_t dim, size_t size,
	size_t lim);

/*	Initialisierung für esh-Interpreter
*/

void SetupRandom (void);

#endif	/* EFEU/Random.h */
