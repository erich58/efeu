/*
Prozesszeit

$Copyright (C) 1996 Erich Frühstück
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
#include <EFEU/cmdsetup.h>
#include <EFEU/tms.h>

#if	HAS_TMS

static int print_tms (const EfiType *type, const void *data, IO *io)
{
	const struct tms *p = data;
	double x = 1. / sysconf(_SC_CLK_TCK);
	return io_xprintf(io, "utime=%.2f+%.2f stime=%.2f+%.2f total=%.2f",
		x * p->tms_utime, x * p->tms_cutime,
		x * p->tms_stime, x * p->tms_cstime,
		x * (p->tms_utime + p->tms_cutime +
			p->tms_stime + p->tms_cstime));
}

EfiType Type_tms = SIMPLE_TYPE("TMS", struct tms, NULL, print_tms);

static void f_create (EfiFunc *func, void *rval, void **arg)
{
	times(rval);
}

static void f_add (EfiFunc *func, void *rval, void **arg)
{
	struct tms *a = arg[0];
	struct tms *b = arg[1];
	struct tms *x = rval;
	x->tms_utime = a->tms_utime + b->tms_utime;
	x->tms_cutime = a->tms_cutime + b->tms_cutime;
	x->tms_stime = a->tms_stime + b->tms_stime;
	x->tms_cstime = a->tms_cstime + b->tms_cstime;
}

static void f_sub (EfiFunc *func, void *rval, void **arg)
{
	struct tms *a = arg[0];
	struct tms *b = arg[1];
	struct tms *x = rval;
	x->tms_utime = a->tms_utime - b->tms_utime;
	x->tms_cutime = a->tms_cutime - b->tms_cutime;
	x->tms_stime = a->tms_stime - b->tms_stime;
	x->tms_cstime = a->tms_cstime - b->tms_cstime;
}

static void f_scale (struct tms *x, struct tms *a, double f)
{
	x->tms_utime = f * a->tms_utime + 0.5;
	x->tms_cutime = f * a->tms_cutime + 0.5;
	x->tms_stime = f * a->tms_stime + 0.5;
	x->tms_cstime = f * a->tms_cstime + 0.5;
}

static void f_rmul (EfiFunc *func, void *rval, void **arg)
{
	f_scale(rval, arg[0], Val_double(arg[1]));
}

static void f_lmul (EfiFunc *func, void *rval, void **arg)
{
	f_scale(rval, arg[1], Val_double(arg[0]));
}

static void f_div (EfiFunc *func, void *rval, void **arg)
{
	f_scale(rval, arg[0], 1. / Val_double(arg[1]));
}

static EfiFuncDef fdef_tms[] = {
	{ 0, &Type_tms, "TMS (void)", f_create },
	{ FUNC_VIRTUAL, &Type_tms, "operator+ (TMS a, TMS b);", f_add },
	{ FUNC_VIRTUAL, &Type_tms, "operator- (TMS a, TMS b);", f_sub },
	{ FUNC_VIRTUAL, &Type_tms, "operator* (TMS t, double f);", f_rmul },
	{ FUNC_VIRTUAL, &Type_tms, "operator* (double f, TMS t);", f_lmul },
	{ FUNC_VIRTUAL, &Type_tms, "operator/ (TMS t, double f);", f_div },
};

#endif

/*	Initialisierung
*/

void CmdSetup_tms(void)
{
#if	HAS_TMS
	AddType(&Type_tms);
	AddFuncDef(fdef_tms, tabsize(fdef_tms));
#else
	;
#endif
}
