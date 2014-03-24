/*
Zeit in Mikrosekunden

$Copyright (C) 2008 Erich Frühstück
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
#include <limits.h>

#if	HAS_TIMEVAL

static int print_timeval (const EfiType *type, const void *data, IO *io)
{
	const struct timeval *p = data;
	return io_xprintf(io, "%u.%06u", (unsigned) p->tv_sec,
		(unsigned) p->tv_usec);
}

EfiType Type_timeval = SIMPLE_TYPE("timeval", struct timeval,
	NULL, print_timeval);

static void f_create (EfiFunc *func, void *rval, void **arg)
{
	gettimeofday(rval, NULL);
}

static void f_dbl2tv (EfiFunc *func, void *rval, void **arg)
{
	struct timeval *tv = rval;
	double t = Val_double(arg[0]);

	if	(t > LONG_MAX)
		t = LONG_MAX;

	tv->tv_sec = t;
	tv->tv_usec = 1000000. * (t - tv->tv_sec);
}

static void f_tv2dbl (EfiFunc *func, void *rval, void **arg)
{
	struct timeval *tv = arg[0];
	Val_double(rval) = tv->tv_sec + 0.000001 * tv->tv_usec;
}

static void f_print (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = print_timeval(&Type_timeval, arg[1], Val_io(arg[0]));
}

static EfiFuncDef fdef_timeval[] = {
	{ 0, &Type_timeval, "timeval (void)", f_create },
	{ 0, &Type_timeval, "timeval (double)", f_dbl2tv },
	{ 0, &Type_double, "timeval ()", f_tv2dbl },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, timeval)", f_print },
};

#endif

/*	Initialisierung
*/

void CmdSetup_timeval(void)
{
#if	HAS_TIMEVAL
	AddType(&Type_timeval);
	AddFuncDef(fdef_timeval, tabsize(fdef_timeval));
#else
	;
#endif
}
