/*
:*:	test floating-point value
:de:	Gleitkommazahlen testen

$Copyright (C) 2004, 2005 Erich Frühstück
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
#include <EFEU/Debug.h>

#define	NOTE	"[TestDef:fcmp]WARNING: " \
	"Comparsion with float value may be unpredictable.\n"

typedef struct {
	int (*test) (double a, double b, double val);
	double a;
	double b;
} VALPAR;

static int test_lt (double a, double b, double val)
{
	return val < a;
}

static int test_le (double a, double b, double val)
{
	return val <= a;
}

static int test_eq (double a, double b, double val)
{
	return val == a;
}

static int test_ne (double a, double b, double val)
{
	return val != a;
}

static int test_ge (double a, double b, double val)
{
	return val >= a;
}

static int test_gt (double a, double b, double val)
{
	return val > a;
}

static int test_range (double a, double b, double val)
{
	return (a <= val && val <= b);
}

static int test_nrange (double a, double b, double val)
{
	return (val < a || b < val);
}


static void *make_par (const char *opt, const char *arg, void *ptr)
{
	VALPAR *par;
	char *p;
	int nflag;

	if	(arg == NULL || *arg == 0 || ptr == NULL)
		return NULL;

	par = memalloc(sizeof(VALPAR));
	nflag = 0;

	if	(*arg == '!' || *arg == '^')
	{
		arg++;
		nflag = 1;
	}
	else	nflag = 0;

	switch (*arg)
	{
	case '<':	par->test = nflag ? test_ge : test_lt; arg++; break;
	case ':':	par->test = nflag ? test_gt : test_le; arg++; break;
	case '>':	par->test = nflag ? test_le : test_gt; arg++; break;
	default:	par->test = NULL; break;
	}

	par->a = C_strtod(arg, &p);
	par->b = par->a;

	if	(par->test)	return par;

	if	(p && *p == ':')
	{
		p++;

		if	(*p)
		{
			par->b = C_strtod(p, &p);
			par->test = nflag ? test_nrange : test_range;
		}
		else	par->test = nflag ? test_lt : test_ge;
	}
	else
	{
		dbg_note("TestPar", NOTE, NULL);
		par->test = nflag ? test_ne : test_eq;
	}

	return par;
}

TestParDef TestPar_double = { &Type_double,
	"=[!^][<>]value]|[first]:[last]",
	make_par, memfree, NULL, 
	":*:The valid values for the test are defined either by a relation\n"
	"(less or greater a specific value) or a intervall (limits enclosed).\n"
	"A ! oder ^ at beginn of the expression negates the test.\n"
	":de:Der Wertebereich für den Test wird entweder durch einen\n"
	"Vergleich (kleiner oder größer als ein vorgegebener Wert)\n"
	"oder durch ein Intervall (Grenzwerte eingeschlossen) festgelegt.\n"
	"Ein ! oder ^ am Anfang der Spezifikation negiert den Test.\n"
};

int Test_double (void *par, double val)
{
	VALPAR *vp = par;
	return vp ? vp->test(vp->a, vp->b, val) : 1;
}
