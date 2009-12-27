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

Type_t Type_Random = REF_TYPE("Random", Random_t *);

#define	Val_Random(data)	((Random_t **) data)[0]

#define	RAND(n)	Val_Random(arg[n])
#define	INT(n)	Val_int(arg[n])
#define	STR(n)	Val_str(arg[n])
#define	DBL(n)	Val_double(arg[n])

CEXPR(f_newrand, Val_Random(rval) = Random(INT(0), GetRandomType(STR(1))))
CEXPR(f_str2rand, Val_Random(rval) = str2Random(STR(0)))
CEXPR(f_copy, Val_Random(rval) = CopyRandom(RAND(0)));

CEXPR(f_seed, SeedRandom(RAND(0), INT(1)))
CEXPR(f_srand, SeedRandom(RAND(1), INT(0)))
CEXPR(f_rand, Val_int(rval) = 2147483648. * UniformRandom(RAND(0)) + 0.5)
CEXPR(f_uniform, Val_double(rval) = UniformRandom(RAND(0)))
CEXPR(f_linear, Val_double(rval) = LinearRandom(RAND(0),DBL(1)))
CEXPR(f_normal, Val_double(rval) = NormalRandom(RAND(0)))
CEXPR(f_prand, Val_int(rval) = PoissonRandom(RAND(0), DBL(1)))
CEXPR(f_rdround, Val_int(rval) = RoundRandom(RAND(0), DBL(1)))

static VarDef_t vdef[] = {
	{ "RandomDebug", &Type_bool, &Random_reftype.debug,
		":*:flag to control debuging of random number generators\n"
		":de:Flag zum Debuggen von Zufallszahlengeneratoren\n" },
};

static FuncDef_t fdef[] = {
	{ FUNC_RESTRICTED, &Type_Random, "str ()", f_str2rand },
	{ 0, &Type_Random, "Random (int seed = 1, str type = NULL)", f_newrand },
	{ 0, &Type_Random, "Random::copy ()", f_copy },
	{ 0, &Type_void, "Random::seed (int value)", f_seed },
	{ 0, &Type_int, "Random::rand ()", f_rand },
	{ 0, &Type_double, "Random::drand ()", f_uniform },
	{ 0, &Type_double, "Random::uniform ()", f_uniform },
	{ 0, &Type_double, "Random::linear (double a)", f_linear },
	{ 0, &Type_double, "Random::normal ()", f_normal },
	{ 0, &Type_int, "Random::poisson (double mw)", f_prand },
	{ 0, &Type_int, "Random::round (double x)", f_rdround },

	{ 0, &Type_int, "rand (Random rand = NULL)", f_rand },
	{ 0, &Type_void, "srand (int val = 1, Random rand = NULL)", f_srand },
	{ 0, &Type_double, "drand (Random rand = NULL)", f_uniform },
	{ 0, &Type_double, "nrand (Random rand = NULL)", f_normal },
};


/*	Initialisierungsfunktion
*/

static void type_info (io_t *io, InfoNode_t *info)
{
	ListRandomType(io);
}

void SetupRandom()
{
	static int setup_done = 0;

	if	(setup_done)	return;

	GetRandomType(NULL);
	AddType(&Type_Random);
	AddVarDef(NULL, vdef, tabsize(vdef));
	AddFuncDef(fdef, tabsize(fdef));
	AddInfo(NULL, "RNG", LBL_RNG, type_info, NULL);
}
