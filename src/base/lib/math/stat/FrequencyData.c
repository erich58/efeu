/*
:*:frequency data
:de:Häufigkeitsdaten

$Copyright (C) 2006 Erich Frühstück
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

#include <Math/StatData.h>
#include <EFEU/stdtype.h>
#include <EFEU/printobj.h>

static int freq_print (const EfiType *st, const void *data, IO *io)
{
	const FrequencyData *freq = data;
	return io_printf(io, "{%.16g, %.16g}", freq->n, freq->x);
}

EfiType Type_FrequencyData = SIMPLE_TYPE("FrequencyData", FrequencyData,
	NULL, freq_print);


static EfiObj *member_n (const EfiObj *base, void *data)
{
	if	(base)
	{
		FrequencyData *f = base->data;
		return LvalObj(&Lval_obj, &Type_double, base, &f->n);
	}
	else	return NULL;
}

static EfiObj *member_x (const EfiObj *base, void *data)
{
	if	(base)
	{
		FrequencyData *f = base->data;
		return LvalObj(&Lval_obj, &Type_double, base, &f->x);
	}
	else	return NULL;
}

static EfiMember var_fd[] = {
	{ "n", &Type_double, member_n, NULL },
	{ "x", &Type_double, member_x, NULL },
};

static void freq_double (EfiFunc *func, void *rval, void **arg)
{
	FrequencyData *freq = rval;
	freq->n = 1.;
	freq->x = Val_double(arg[0]);
}

static void freq_list (EfiFunc *func, void *rval, void **arg)
{
	FrequencyData *freq = rval;
	EfiObjList *list = Val_ptr(arg[0]);
	freq->n = list ? Obj2double(RefObj(list->obj)) : 0.;
	freq->x = list->next ? Obj2double(RefObj(list->next->obj)) : 0.;
}

static void freq_to_list (EfiFunc *func, void *rval, void **arg)
{
	FrequencyData *freq = arg[0];
	Val_ptr(rval) = MakeObjList(2, double2Obj(freq->n),
		double2Obj(freq->x));
}

static void freq_fprint (EfiFunc *func, void *rval, void **arg)
{
	IO *out = Val_io(arg[0]);
	FrequencyData *freq = arg[1];
	int n = io_puts(PrintListBegin, out);
	n += PrintDouble(out, freq->n);
	n += io_puts(PrintListDelim, out);
	n += PrintDouble(out, freq->x);
	n += io_puts(PrintListEnd, out);
	Val_int(rval) = n;
}

static void binary_add (EfiFunc *func, void *rval, void **arg)
{
	FrequencyData *freq = rval;
	FrequencyData *a = arg[0];
	FrequencyData *b = arg[1];
	freq->n = a->n + b->n;
	freq->x = a->x + b->x;
}

static void assign_add (EfiFunc *func, void *rval, void **arg)
{
	FrequencyData *a = arg[0];
	FrequencyData *b = arg[1];
	a->n += b->n;
	a->x += b->x;
}


static EfiFuncDef func[] = {
	{ FUNC_VIRTUAL, &Type_FrequencyData,
		"FrequencyData (double)", freq_double },
	{ FUNC_VIRTUAL, &Type_FrequencyData,
		"FrequencyData (List_t)", freq_list },
	{ FUNC_VIRTUAL|FUNC_RESTRICTED, &Type_list,
		"FrequencyData ()", freq_to_list },
	{ FUNC_VIRTUAL, &Type_int,
		"fprint (IO, FrequencyData)", freq_fprint },
	{ FUNC_VIRTUAL, &Type_FrequencyData, \
		"operator+ (FrequencyData, FrequencyData)", binary_add },
	{ 0, &Type_FrequencyData, \
		"& FrequencyData::operator+= & (FrequencyData)", assign_add },
};

void CmdSetup_FrequencyData (void)
{
	AddType(&Type_FrequencyData);
	AddEfiMember(Type_FrequencyData.vtab, var_fd, tabsize(var_fd));
	AddFuncDef(func, tabsize(func));
}
