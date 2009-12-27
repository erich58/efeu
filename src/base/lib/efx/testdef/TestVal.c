/*
:*:	test unsigned integer value
:de:	Vorzeichenfreien Ganzzahlwert prüfen

(c) 1997, 2005 Erich Frühstück
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

#define EN_DAT ":*:" \
"The selection is defined by a single date, a range below or\n" \
"above a date (-date, +date) or a time intervall (first\\:last).\n" \
"Time specifications have the form YYYY-MM-DD or DD.MM.YYYY.\n" \
"A missing month or day is replaced by the first month/day. On the end\n" \
"of a intervall, missing values are replaced by the last month/day.\n"

#define	EN_NEG \
"A ! oder ^ at beginn of the expression negates the test.\n"

#define	DE_VAL ":de:" \
"Die Selektion wird durch einen einzelnen Wert, durch einen Bereich\n"\
"unterhalb oder oberhalb eines Wertes (-value, +value) oder durch\n"\
"ein Intervall (first\\:last) festgelegt.\n"

#define	DE_DAT ":de:" \
"Die Selektion wird durch ein einzelnes Datum, durch einen Bereich\n"\
"unterhalb oder oberhalb eines Datums (-date, +date) oder durch\n"\
"einen Zeitbereich (beg\\:end) festgelegt. Datumsangaben haben die\n" \
"Form JJJJ-MM-TT oder TT.MM.JJJJ. Fehlende Tages- und Monatsangaben\n" \
"werden durch das erste Monat/den ersten Tag ergänzt. Beim Endedatum\n"\
"eines Zeitbereichs wird das letzte Monat/der letzte Tag verwendet.\n"

#define	DE_NEG \
	"Ein ! oder ^ am Anfang der Spezifikation negiert den Test.\n"


typedef struct {
	int (*test) (unsigned a, unsigned b, unsigned val);
	unsigned a;
	unsigned b;
} VALPAR;

static int test_lt (unsigned a, unsigned b, unsigned val)
{
	return val < a;
}

static int test_le (unsigned a, unsigned b, unsigned val)
{
	return val <= a;
}

static int test_eq (unsigned a, unsigned b, unsigned val)
{
	return val == a;
}

static int test_ne (unsigned a, unsigned b, unsigned val)
{
	return val != a;
}

static int test_ge (unsigned a, unsigned b, unsigned val)
{
	return val >= a;
}

static int test_gt (unsigned a, unsigned b, unsigned val)
{
	return val > a;
}

static int test_range (unsigned a, unsigned b, unsigned val)
{
	return (a <= val && val <= b);
}

static int test_nrange (unsigned a, unsigned b, unsigned val)
{
	return (val < a || b < val);
}


void *MakeValPar (const char *opt, const char *arg, void *ptr)
{
	VALPAR *par;
	unsigned (*get)(char *arg, char **p, int flag);
	char *p;
	int nflag;

	if	(arg == NULL || *arg == 0 || ptr == NULL)
		return NULL;

	get = ptr;
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

static unsigned get_val(char *def, char **ptr, int flag)
{
	return strtoul(def, ptr, 0);
}

static unsigned get_dat(char *def, char **ptr, int flag)
{
	return str2Calendar(def, ptr, flag);
}

static unsigned get_dat1900(char *def, char **ptr, int flag)
{
	return CalBase1900(str2Calendar(def, ptr, flag));
}

TestParDef TestPar_Num = { &Type_uint,
	"=[!^][+-]value|[first]:[last]",
	MakeValPar, memfree, get_val,
	EN_VAL EN_NEG DE_VAL DE_NEG
};

TestParDef TestPar_Dat = { &Type_Date,
	"=[!^][+-]date|[beg]:[end]",
	MakeValPar, memfree, get_dat,
	EN_DAT EN_NEG DE_DAT DE_NEG
};

TestParDef TestPar_Dat1900 = { &Type_uint,
	"=[!^][+-]date|[beg]:[end]",
	MakeValPar, memfree, get_dat1900,
	EN_DAT EN_NEG DE_DAT DE_NEG
};

int TestVal (void *par, unsigned val)
{
	register VALPAR *vp = par;
	return vp ? vp->test(vp->a, vp->b, val) : 1;
}
