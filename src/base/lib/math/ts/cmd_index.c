/*
:*:set up interpreter to use time series index 
:de:Befehlsinterpreter für Zeitreihenindizes initialisieren

$Copyright (C) 1997 Erich Frühstück
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

#include <EFEU/object.h>
#include <EFEU/stdtype.h>
#include <Math/TimeSeries.h>

EfiType Type_TimeIndex = SIMPLE_TYPE("TimeIndex", TimeIndex, NULL);

/*	Komponentenfunktionen
*/

static EfiObj *idx_length (const EfiObj *base, void *data)
{
	int x = base ? tindex_length(*((TimeIndex *) base->data)) : 0;
	return NewObj(&Type_int, &x);
}

static EfiObj *idx_year (const EfiObj *base, void *data)
{
	int x = base ? tindex_year(*((TimeIndex *) base->data)) : 0;
	return NewObj(&Type_int, &x);
}

static EfiObj *idx_month (const EfiObj *base, void *data)
{
	int x = base ? tindex_month(*((TimeIndex *) base->data)) : 0;
	return NewObj(&Type_int, &x);
}

static EfiObj *idx_beg (const EfiObj *base, void *data)
{
	int x = base ? tindex_floor(*((TimeIndex *) base->data)) : 0;
	return NewObj(&Type_int, &x);
}

static EfiObj *idx_end (const EfiObj *base, void *data)
{
	int x = base ? tindex_ceil(*((TimeIndex *) base->data)) : 0;
	return NewObj(&Type_int, &x);
}

static EfiObj *idx_floor (const EfiObj *base, void *data)
{
	double x = base ? TimeIndex2dbl(*((TimeIndex *) base->data), 0, 0) : 0.;
	return NewObj(&Type_double, &x);
}

static EfiObj *idx_ceil (const EfiObj *base, void *data)
{
	double x = base ? TimeIndex2dbl(*((TimeIndex *) base->data), 0, 2) : 0.;
	return NewObj(&Type_double, &x);
}

static EfiMember member[] = {
	{ "length", &Type_int, idx_length, NULL },
	{ "year", &Type_int, idx_year, NULL },
	{ "month", &Type_int, idx_month, NULL },
	{ "beg", &Type_Date, idx_beg, NULL },
	{ "end", &Type_Date, idx_end, NULL },
	{ "floor", &Type_double, idx_floor, NULL },
	{ "ceil", &Type_double, idx_ceil, NULL },
};

#define	IDX(n)		Val_TimeIndex(arg[n])
#define	RVIDX		Val_TimeIndex(rval)

#define	CMPIDX(name, op)	\
static void name (EfiFunc *func, void *rval, void **arg)	\
{ Val_bool(rval) = tindex_diff(IDX(1), IDX(0)) op 0; };

CMPIDX(b_idx_lt, <)
CMPIDX(b_idx_le, <=)
CMPIDX(b_idx_eq, ==)
CMPIDX(b_idx_ne, !=)
CMPIDX(b_idx_ge, >=)
CMPIDX(b_idx_gt, >)

static void f_add_idx (EfiFunc *func, void *rval, void **arg)
{
	RVIDX = IDX(0);
	RVIDX.value += Val_int(arg[1]);
}

static void f_sub_idx (EfiFunc *func, void *rval, void **arg)
{
	RVIDX = Val_TimeIndex(arg[0]);
	RVIDX.value -= Val_int(arg[1]);
}

static void f_diff_idx (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = tindex_diff(IDX(1), IDX(0));
}


static void f_str2idx (EfiFunc *func, void *rval, void **arg)
{
	RVIDX = str2TimeIndex(Val_str(arg[0]));
}

static void f_ptr2idx (EfiFunc *func, void *rval, void **arg)
{
	RVIDX.type = 0;
	RVIDX.value = 0;
}

static void f_konv (EfiFunc *func, void *rval, void **arg)
{
	fprintf(stderr, "WARNING: konv instead of conv.\n");
	RVIDX = tindex_conv(IDX(0), Val_char(arg[1]), Val_int(arg[2]));
}

static void f_conv (EfiFunc *func, void *rval, void **arg)
{
	char *mode = Val_str(arg[1]);

	if	(mode)
	{
		int type = 0;
		int pos = 0;

		for (; *mode != 0; mode++)
		{
			switch (*mode)
			{
			case '+':	pos = 1; break;
			case '-':	pos = -1; break;
			default:	type = tindex_type(mode); break;
			}
		}

		RVIDX = tindex_conv(IDX(0), type, pos);
	}
	else	RVIDX = IDX(0);
}

static void f_xkonv (EfiFunc *func, void *rval, void **arg)
{
	fprintf(stderr, "WARNING: konv instead of conv.\n");
	f_conv(func, rval, arg);
}


static void f_fprint_idx (EfiFunc *func, void *rval, void **arg)
{
	char *s = TimeIndex2str(IDX(1), 0);
	Val_int(rval) = io_puts(s, Val_io(arg[0]));
	memfree(s);
}

static void f_idx2str (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = TimeIndex2str(IDX(0), 0);
}

static void f_idx2dbl (EfiFunc *func, void *rval, void **arg)
{
	Val_double(rval) = TimeIndex2dbl(IDX(0), 0, 1);
}

static EfiFuncDef func[] = {
	{ 0, &Type_TimeIndex, "_Ptr_ ()", f_ptr2idx },
	{ 0, &Type_TimeIndex, "str ()", f_str2idx },
	{ FUNC_VIRTUAL, &Type_TimeIndex, "TimeIndex::konv (char type, int pos = 0)",
		f_konv },
	{ FUNC_VIRTUAL, &Type_TimeIndex, "TimeIndex::konv (str mode)", f_xkonv },
	{ FUNC_VIRTUAL, &Type_TimeIndex, "TimeIndex::conv (str mode)", f_conv },
	{ FUNC_RESTRICTED, &Type_str, "TimeIndex ()", f_idx2str },
	{ FUNC_RESTRICTED, &Type_double, "TimeIndex ()", f_idx2dbl },

	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, TimeIndex)", f_fprint_idx },

	{ FUNC_VIRTUAL, &Type_TimeIndex, "operator+ (TimeIndex *x, int n)",
		f_add_idx },
	{ FUNC_VIRTUAL, &Type_TimeIndex, "operator- (TimeIndex *x, int n)",
		f_sub_idx },
	{ FUNC_VIRTUAL, &Type_int, "operator- (TimeIndex *a, TimeIndex *b)",
		f_diff_idx },

	{ FUNC_VIRTUAL, &Type_bool, "operator< (TimeIndex, TimeIndex)",
		b_idx_lt },
	{ FUNC_VIRTUAL, &Type_bool, "operator<= (TimeIndex, TimeIndex)",
		b_idx_le },
	{ FUNC_VIRTUAL, &Type_bool, "operator== (TimeIndex, TimeIndex)",
		b_idx_eq },
	{ FUNC_VIRTUAL, &Type_bool, "operator!= (TimeIndex, TimeIndex)",
		b_idx_ne },
	{ FUNC_VIRTUAL, &Type_bool, "operator>= (TimeIndex, TimeIndex)",
		b_idx_ge },
	{ FUNC_VIRTUAL, &Type_bool, "operator> (TimeIndex, TimeIndex)",
		b_idx_gt },
	{ FUNC_VIRTUAL, &Type_list, "operator: (TimeIndex, TimeIndex)",
		RangeFunc },
	{ FUNC_VIRTUAL, &Type_list, "operator: (TimeIndex, TimeIndex, int)",
		RangeFunc },
};


void CmdSetup_TimeIndex (void)
{
	AddType(&Type_TimeIndex);
	AddFuncDef(func, tabsize(func));
	AddEfiMember(Type_TimeIndex.vtab, member, tabsize(member));
}
