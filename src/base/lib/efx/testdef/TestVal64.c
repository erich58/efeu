/*
:*:	test uint64_t integer value
:de:	Vorzeichenfreien 64-Bit Ganzzahlwert prüfen

(c) 1997, 2007 Erich Frühstück
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

#include <EFEU/TestDef.h>
#include <EFEU/calendar.h>

#define	EN_VAL  ":*:" \
"The selection is defined by a single value, a range below or\n" \
"above a value (-value, +value) or a intervall (first\\:last).\n"

#define	EN_NEG \
"A ! oder ^ at beginn of the expression negates the test.\n"

#define	DE_VAL ":de:" \
"Die Selektion wird durch einen einzelnen Wert, durch einen Bereich\n"\
"unterhalb oder oberhalb eines Wertes (-value, +value) oder durch\n"\
"ein Intervall (first\\:last) festgelegt.\n"

#define	DE_NEG \
	"Ein ! oder ^ am Anfang der Spezifikation negiert den Test.\n"


typedef struct {
	int (*test) (uint64_t a, uint64_t b, uint64_t val);
	uint64_t a;
	uint64_t b;
} VAL64;

static int test_lt (uint64_t a, uint64_t b, uint64_t val)
{
	return val < a;
}

static int test_le (uint64_t a, uint64_t b, uint64_t val)
{
	return val <= a;
}

static int test_eq (uint64_t a, uint64_t b, uint64_t val)
{
	return val == a;
}

static int test_ne (uint64_t a, uint64_t b, uint64_t val)
{
	return val != a;
}

static int test_ge (uint64_t a, uint64_t b, uint64_t val)
{
	return val >= a;
}

static int test_gt (uint64_t a, uint64_t b, uint64_t val)
{
	return val > a;
}

static int test_range (uint64_t a, uint64_t b, uint64_t val)
{
	return (a <= val && val <= b);
}

static int test_nrange (uint64_t a, uint64_t b, uint64_t val)
{
	return (val < a || b < val);
}


static void *MakePar64 (const char *opt, const char *arg, void *ptr)
{
	VAL64 *par;
	uint64_t (*get)(char *arg, char **p, int flag);
	char *p;
	int nflag;

	if	(arg == NULL || *arg == 0 || ptr == NULL)
		return NULL;

	get = ptr;
	par = memalloc(sizeof(VAL64));
	nflag = 0;

	if	(*arg == '!' || *arg == '^')
	{
		arg++;
		nflag = 1;
	}
	else	nflag = 0;

	switch (*arg)
	{
	case '-':	par->test = nflag ? test_ge : test_lt; arg++; break;
	case ':':	par->test = nflag ? test_gt : test_le; arg++; break;
	case '+':	par->test = nflag ? test_le : test_gt; arg++; break;
	default:	par->test = NULL; break;
	}

	par->a = get((char *) arg, &p, 0);
	par->b = par->a;

	if	(par->test)	return par;

	if	(p && *p == ':')
	{
		p++;

		if	(*p)
		{
			par->b = get(p, &p, 1);
			par->test = nflag ? test_nrange : test_range;
		}
		else	par->test = nflag ? test_lt : test_ge;
	}
	else	par->test = nflag ? test_ne : test_eq;

	return par;
}

static uint64_t get_val (char *def, char **ptr, int flag)
{
	return mstr2uint64(def, ptr, 0);
}


TestParDef TestPar_64 = { &Type_uint64,
	"=[!^][+-]value|[first]:[last]",
	MakePar64, memfree, get_val,
	EN_VAL EN_NEG DE_VAL DE_NEG
};

int TestVal64 (void *par, uint64_t val)
{
	VAL64 *vp = par;
	return vp ? vp->test(vp->a, vp->b, val) : 1;
}
