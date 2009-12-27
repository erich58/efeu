/*	Implementation des Zufallszahlengenerator
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/


#include <EFEU/efutil.h>
#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/Random.h>
#include <math.h>

#ifndef	M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif

static char *rident (Random_t *rd)
{
	strbuf_t *sb;
	uchar_t *ptr;
	int i;

	sb = new_strbuf(0);

	for (i = 0, ptr = rd->state; i < rd->size; i++, ptr++)
	{
		if	(i && i % 8 == 0)
			sb_putc(' ', sb);

		sb_printf(sb, "%02x", *ptr);
	}

	return sb2str(sb);
}

static Random_t *radmin (Random_t *tg, const Random_t *src)
{
	if	(tg)
	{
		memfree(tg->state);
		memfree(tg);
		return NULL;
	}
	else	return memalloc(sizeof(Random_t));
}

REFTYPE(Random_reftype, "Random", rident, radmin);

Random_t *Random (int sval, int dim)
{
	Random_t *rand;

	if	(dim <= 8)	dim = 8;
	else if	(dim <= 32)	dim = 32;
	else if	(dim <= 64)	dim = 64;
	else if	(dim <= 128)	dim = 128;
	else			dim = 256;

	rand = rd_create(&Random_reftype);
	rand->state = memalloc(dim);
	rand->size = dim;
	setstate(initstate((unsigned) sval, rand->state, dim));
	return rand;
}


void SeedRandom (Random_t *rd, int sval)
{
	if	(rd)
	{
		void *save = (void *) setstate(rd->state);
		srandom(sval);
		setstate(save);
	}
	else	srandom(sval);
}


/*	Zufallszahlen
*/

long LongRandom (Random_t *rd)
{
	if	(rd)
	{
		void *save = (void *) setstate(rd->state);
		long x = random();
		setstate(save);
		return x;
	}

	return random();
}

double UniformRandom (Random_t *rd)
{
	return DRAND_KOEF * LongRandom(rd);
}

size_t RandomIndex (Random_t *rd, size_t dim)
{
	size_t k = dim * DRAND_KOEF * LongRandom(rd);
	return (k < dim) ? k : dim - 1;
}

double LinearRandom (Random_t *rd, double z)
{
	double x = DRAND_KOEF * LongRandom(rd);

	if	(z == 0.)	return x;
	if	(z >= 1.)	return sqrt(x);
	if	(z <= -1.)	return 1. - sqrt(1. - x);

	return 0.5 * (z - 1. + sqrt(z * (z - 2. + 4. * x) + 1.)) / z;
}

double NormalRandom (Random_t *rd)
{
	void *save;
	double x;

	save = rd ? (void *) setstate(rd->state): NULL;
	x = sqrt(-2. * log(DRAND_KOEF * random()));
	x *= cos(M_PI * 2. * DRAND_KOEF * random());

	if	(save)	setstate(save);

	return x;
}

/*	Poissonverteilung: bei großem Mittelwert wird Normalverteilung
	verwendet. Ab 9 annähernd Normalverteilt, ab 700 Überlauf.
*/

#define	POISSON_MAX	20.5 	/* 9 <= x <= 700 */

int PoissonRandom (Random_t *rd, double mw)
{
	void *save;
	double x, lim;
	int n;

	if	(mw > POISSON_MAX)
		return (int) (mw + sqrt(mw) * NormalRandom(rd) + 0.5);

	save = rd ? (void *) setstate(rd->state): NULL;
	lim = exp(-mw);

	for (n = 0, x = 1.; x > lim; n++)
		x *= DRAND_KOEF * random();

	return n ? n - 1 : 0;
}


/*	Zufallsrundung
*/

int RoundRandom(Random_t *rd, double x)
{
	int val;
	double rval;

	val = (int) x;
	x -= val;
	rval = UniformRandom(rd);

	if	(x >= 0.)
	{
		if	(x > rval)	val++;
	}
	else	if	(-x > rval)	val--;

	return val;
}


/*	Schnittstelle zum esh-Interpreter
*/

Type_t Type_Random = REF_TYPE("Random", Random_t *);

#define	Val_Random(data)	((Random_t **) data)[0]

#define	RAND(n)	Val_Random(arg[n])
#define	INT(n)	Val_int(arg[n])
#define	DBL(n)	Val_double(arg[n])

CEXPR(f_newrand, Val_Random(rval) = Random(INT(0), INT(1)))

static void f_copy (Func_t *func, void *rval, void **arg)
{
	Random_t *src = Val_Random(arg[0]);

	if	(src)
	{
		Random_t *tg = Random(1, src->size);
		memcpy(tg->state, src->state, src->size);
		Val_Random(rval) = tg;
	}
	else	Val_Random(rval) = NULL;
}

CEXPR(f_seed, SeedRandom(RAND(0), INT(1)))
CEXPR(f_srand, SeedRandom(RAND(1), INT(0)))
CEXPR(f_rand, Val_int(rval) = LongRandom(RAND(0)))
CEXPR(f_uniform, Val_double(rval) = UniformRandom(RAND(0)))
CEXPR(f_linear, Val_double(rval) = LinearRandom(RAND(0),DBL(1)))
CEXPR(f_normal, Val_double(rval) = NormalRandom(RAND(0)))
CEXPR(f_prand, Val_int(rval) = PoissonRandom(RAND(0), DBL(1)))
CEXPR(f_rdround, Val_int(rval) = RoundRandom(RAND(0), DBL(1)))

static Var_t vdef[] = {
	{ "RandomDebug", &Type_bool, &Random_reftype.debug },
};

static FuncDef_t fdef[] = {
	{ 0, &Type_Random, "Random (int seed = 1, int dim = 8)", f_newrand },
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

void SetupRandom()
{
	static int setup_done = 0;
	static char rstate[8];

	if	(setup_done)	return;

	initstate(1, rstate, sizeof(rstate));
	AddType(&Type_Random);
	AddVar(NULL, vdef, tabsize(vdef));
	AddFuncDef(fdef, tabsize(fdef));
}
