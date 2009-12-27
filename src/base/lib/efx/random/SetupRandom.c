/*
Verfügbarmachung des Zufallszahlengenerators für esh-Interpreter

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/Random.h>
#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/Info.h>

#define	LBL_RNG	\
	":*:random number generators" \
	":de:Zufallszahlengeneratoren"

EfiType Type_Random = REF_TYPE("Random", Random *);

#define	Val_Random(data)	((Random **) data)[0]

#define	RAND(n)	Val_Random(arg[n])
#define	INT(n)	Val_int(arg[n])
#define	STR(n)	Val_str(arg[n])
#define	DBL(n)	Val_double(arg[n])

CEXPR(f_null, Val_Random(rval) = NULL)
CEXPR(f_newrand, Val_Random(rval) = NewRandom(GetRandomType(STR(1)), INT(0)))
CEXPR(f_int2rand, Val_Random(rval) = NewRandom(NULL, INT(0)))
CEXPR(f_str2rand, Val_Random(rval) = str2Random(STR(0)))
CEXPR(f_copy, Val_Random(rval) = CopyRandom(RAND(0)));

CEXPR(f_seed, SeedRandom(RAND(0), INT(1)))
CEXPR(f_srand, SeedRandom(RAND(1), INT(0)))
CEXPR(f_rand, Val_int(rval) = 2147483648. * UniformRandom(RAND(0)) + 0.5)
CEXPR(f_uniform, Val_double(rval) = UniformRandom(RAND(0)))
CEXPR(f_linear, Val_double(rval) = LinearRandom(RAND(0),DBL(1)))
CEXPR(f_normal, Val_double(rval) = NormalRandom(RAND(0)))
CEXPR(f_prand, Val_int(rval) = PoissonRandom(RAND(0), DBL(1)))
CEXPR(f_rdround, Val_int(rval) = RandomRound(RAND(0), DBL(1)))
CEXPR(f_rindex, Val_int(rval) = RandomIndex(RAND(0), Val_uint(arg[1])))

static void f_choice (EfiFunc *func, void *rval, void **arg)
{
	EfiVec *vec = Val_ptr(arg[1]);
	Val_int(rval) = RandomChoice(RAND(0), vec->buf.data, vec->buf.used,
		vec->buf.elsize, Val_uint(arg[2]));
}

static EfiFuncDef fdef[] = {
	{ FUNC_RESTRICTED, &Type_Random, "_Ptr_ ()", f_null },
	{ 0, &Type_Random, "Random (str rdef)", f_str2rand },
	{ 0, &Type_Random, "Random (str type, int seed)", f_newrand },
	{ 0, &Type_Random, "Random (int seed)", f_int2rand },
	{ 0, &Type_Random, "Random::copy ()", f_copy },
	{ 0, &Type_void, "Random::seed (int value)", f_seed },
	{ 0, &Type_int, "Random::rand ()", f_rand },
	{ 0, &Type_double, "Random::drand ()", f_uniform },
	{ 0, &Type_double, "Random::uniform ()", f_uniform },
	{ 0, &Type_double, "Random::linear (double a)", f_linear },
	{ 0, &Type_double, "Random::normal ()", f_normal },
	{ 0, &Type_int, "Random::poisson (double mw)", f_prand },
	{ 0, &Type_int, "Random::round (double x)", f_rdround },
	{ 0, &Type_int, "Random::index (unsigned x)", f_rindex },
	{ 0, &Type_int, "Random::choice (EfiVec vec, unsigned lim)", f_choice },

	{ 0, &Type_int, "rand (Random rand = NULL)", f_rand },
	{ 0, &Type_void, "srand (int val = 1, Random rand = NULL)", f_srand },
	{ 0, &Type_double, "drand (Random rand = NULL)", f_uniform },
	{ 0, &Type_double, "nrand (Random rand = NULL)", f_normal },
};


/*	Initialisierungsfunktion
*/

static void type_info (IO *io, InfoNode *info)
{
	ListRandomType(io);
}

void SetupRandom()
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;

	GetRandomType(NULL);
	AddType(&Type_Random);
	AddFuncDef(fdef, tabsize(fdef));
	AddInfo(NULL, "RNG", LBL_RNG, type_info, NULL);
}
