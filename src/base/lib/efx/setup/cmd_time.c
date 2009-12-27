/*
Zeitstempel

$Copyright (C) 2009 Erich Frühstück
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
#include <EFEU/calendar.h>
#include <EFEU/EfiClass.h>
#include <limits.h>
#include <time.h>

static int print_time (const EfiType *type, const void *data, IO *io)
{
	const time_t *t = data;
	return io_xprintf(io, "%llu", (int64_t) *t);
}

EfiType Type_time = SIMPLE_TYPE("time", time_t, NULL, print_time);

static char *time_fmt = NULL;

#define	DM_IDX	0
#define	DM_DAY	1
#define	DM_MON	2
#define	DM_YEAR	3
#define	DM_YDAY	4
#define	DM_WDAY	5
#define	DM_HOUR	6
#define	DM_MIN	7
#define	DM_SEC	8
#define	DM_DST	9

static EfiObj *time2obj (time_t *t, int type)
{
	struct tm *tm = localtime(t);
	int val;

	switch (type)
	{
	case DM_DAY:	val = tm->tm_mday; break;
	case DM_MON:	val = tm->tm_mon + 1; break;
	case DM_YEAR:	val = tm->tm_year + 1900; break;
	case DM_YDAY:	val = tm->tm_yday + 1; break;
	case DM_WDAY:	val = tm->tm_wday; break;
	case DM_HOUR:	val = tm->tm_hour; break;
	case DM_MIN:	val = tm->tm_min; break;
	case DM_SEC:	val = tm->tm_sec; break;
	case DM_DST:	return bool2Obj(tm->tm_isdst);
	default:	val = 0; break;
	}

	return int2Obj(val);
}

static EfiObj *time_member (const EfiObj *obj, void *data)
{
	return obj ? time2obj(obj->data,
		(int) (size_t) ((EfiMember *) data)->par) : int2Obj(0);
}

static EfiVarDef v_time[] = {
	{ "fmt", &Type_str, &time_fmt },
};

static EfiMember m_time[] = {
	{ "day", &Type_int, time_member, (void *) DM_DAY },
	{ "month", &Type_int, time_member, (void *) DM_MON },
	{ "year", &Type_int, time_member, (void *) DM_YEAR },
	{ "wday", &Type_int, time_member, (void *) DM_WDAY },
	{ "yday", &Type_int, time_member, (void *) DM_YDAY },
	{ "hour", &Type_int, time_member, (void *) DM_HOUR },
	{ "min", &Type_int, time_member, (void *) DM_MIN },
	{ "sec", &Type_int, time_member, (void *) DM_SEC },
	{ "dst", &Type_bool, time_member, (void *) DM_DST },
};

static void f_create (EfiFunc *func, void *rval, void **arg)
{
	time(rval);
}

#define	CONV(name, from, to)	\
static void name (EfiFunc *func, void *rval, void **arg) \
{ *((to *) rval) = *((from *) arg[0]); }

CONV(c_int, time_t, int)
CONV(c_int32, time_t, int32_t)
CONV(c_int64, time_t, int64_t)

CONV(from_int, int, time_t)
CONV(from_int64, int64_t, time_t)

CONV(c_uint, time_t, unsigned int)
CONV(c_uint32, time_t, uint32_t)
CONV(c_uint64, time_t, uint64_t)

CONV(from_uint64, uint64_t, time_t)

CONV(c_float, time_t, float)
CONV(c_double, time_t, double)

CONV(from_double, double, time_t)

static void f_to_str (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = tm2str(Val_str(arg[1]), localtime(arg[0]));
}

static void f_fprint (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = Print_tm(Val_io(arg[0]),
		time_fmt, localtime(arg[1]));
}

static EfiFuncDef fdef_time[] = {
	{ 0, &Type_time, "time (void)", f_create },
	{ 0, &Type_time, "time (int)", from_int },
	{ 0, &Type_time, "time (varint)", from_int64 },
	{ 0, &Type_time, "time (varsize)", from_uint64 },
	{ 0, &Type_time, "time (double)", from_double },

	{ 0, &Type_int, "time ()", c_int },
	{ 0, &Type_int32, "time ()", c_int32 },
	{ 0, &Type_int64, "time ()", c_int64 },
	{ 0, &Type_varint, "time ()", c_int64 },

	{ 0, &Type_uint, "time ()", c_uint },
	{ 0, &Type_uint32, "time ()", c_uint32 },
	{ 0, &Type_uint64, "time ()", c_uint64 },
	{ 0, &Type_varsize, "time ()", c_uint64 },

	{ 0, &Type_float, "time ()", c_float },
	{ 0, &Type_double, "time ()", c_double },
	{ 0, &Type_str, "str (time t, str fmt = NULL)", f_to_str },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, time)", f_fprint },
//	{ 0, &Type_int, "time::print (IO out, str fmt = NULL)", f_time_print },
};

/*	Klassifikation
*/

static int get_sec (const EfiObj *obj)
{
	struct tm *tm = localtime(obj->data);
	return (tm->tm_hour * 60 + tm->tm_min) * 60 + tm->tm_sec;
}

static EfiClass DayTime_class = EFI_CLASS(NULL,
	"daytime", "[fmt]=unit[,range]", DayTime, get_sec,
	"daytime classification"
);

static int get_cal (const EfiObj *obj)
{
	struct tm *tm = localtime(obj->data);
	return CalendarIndex(tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900);
}

static EfiClass Calendar_class = EFI_CLASS(NULL,
	"date", "[fmt]=ug[:og]", CalClass, get_cal,
	"calendar classification"
);

/*	Initialisierung
*/

void CmdSetup_time(void)
{
	AddType(&Type_time);
	AddVarDef(Type_time.vtab, v_time, tabsize(v_time));
	AddEfiMember(Type_time.vtab, m_time, tabsize(m_time));
	AddFuncDef(fdef_time, tabsize(fdef_time));
	AddEfiPar(&Type_time, &DayTime_class);
	AddEfiPar(&Type_time, &Calendar_class);
}
