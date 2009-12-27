/*
Hilfsprogramme

$Copyright (C) 1994 Erich Fr�hst�ck
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

#include <EFEU/pconfig.h>
#include <EFEU/efutil.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/calendar.h>
#include <time.h>

#define	RVINT	Val_int(rval)
#define	RVSTR	Val_str(rval)

#define	STR(n)		Val_str(arg[n])
#define	INT(n)		Val_int(arg[n])
#define	LONG(n)		Val_long(arg[n])
#define	DOUBLE(n)	Val_double(arg[n])
#define	TIME(x)		((CalTimeIndex *) (x))[0]


/*	Datums- und Zeittype
*/

EfiType Type_Date = SIMPLE_TYPE("Date", int, &Type_int);
EfiType Type_Time = SIMPLE_TYPE("Time", CalTimeIndex, NULL);

static char *Date_fmt = NULL;
static char *Time_fmt = NULL;

#define	DM_IDX	0
#define	DM_DAY	1
#define	DM_MON	2
#define	DM_YEAR	3
#define	DM_YDAY	4
#define	DM_WDAY	5
#define	DM_HOUR	6
#define	DM_MIN	7
#define	DM_SEC	8

static EfiObj *cal2obj (CalInfo *cal, int type)
{
	int val;

	if	(cal == NULL)
		cal = Calendar(0, NULL);

	switch (type)
	{
	case DM_DAY:	val = cal->day; break;
	case DM_MON:	val = cal->month; break;
	case DM_YEAR:	val = cal->year; break;
	case DM_YDAY:	val = cal->yday; break;
	case DM_WDAY:	val = cal->wday; break;
	case DM_HOUR:	val = cal->hour; break;
	case DM_MIN:	val = cal->min; break;
	case DM_SEC:	val = cal->sec; break;
	default:	val = 0; break;
	}

	return int2Obj(val);
}

static EfiObj *base1900 (const EfiObj *obj, void *data)
{
	return int2Obj(obj ? CalBase1900(Val_Date(obj->data)) : 0);
}

static EfiObj *Date_member (const EfiObj *obj, void *data)
{
	return obj ? cal2obj(Calendar(Val_Date(obj->data), NULL),
		(int) (size_t) ((EfiMember *) data)->par) : int2Obj(0);
}

static EfiVarDef v_Date[] = {
	{ "fmt", &Type_str, &Date_fmt },
};

static EfiMember m_Date[] = {
	{ "day", &Type_int, Date_member, (void *) DM_DAY },
	{ "month", &Type_int, Date_member, (void *) DM_MON },
	{ "year", &Type_int, Date_member, (void *) DM_YEAR },
	{ "wday", &Type_int, Date_member, (void *) DM_WDAY },
	{ "yday", &Type_int, Date_member, (void *) DM_YDAY },
	{ "cal1900", &Type_int, base1900, NULL },
};

static EfiObj *Time_member (const EfiObj *obj, void *data)
{
	return obj ? cal2obj(TimeCalendar(TIME(obj->data), NULL),
		(int) (size_t) ((EfiMember *) data)->par) : int2Obj(0);
}

static EfiVarDef v_Time[] = {
	{ "fmt", &Type_str, &Date_fmt },
};

static EfiMember m_Time[] = {
	{ "day", &Type_int, Time_member, (void *) DM_DAY },
	{ "month", &Type_int, Time_member, (void *) DM_MON },
	{ "year", &Type_int, Time_member, (void *) DM_YEAR },
	{ "wday", &Type_int, Time_member, (void *) DM_WDAY },
	{ "yday", &Type_int, Time_member, (void *) DM_YDAY },
	{ "hour", &Type_int, Time_member, (void *) DM_HOUR },
	{ "min", &Type_int, Time_member, (void *) DM_MIN },
	{ "sec", &Type_int, Time_member, (void *) DM_SEC },
};


/*	Datumsfunktionen
*/

static void f_fprint_Date (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = PrintCalendar(Val_io(arg[0]),
		Date_fmt, Val_Date(arg[1]));
}

static void f_Date2str (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = Calendar2str(Date_fmt, Val_Date(arg[0]));
}

static void f_Date_konv (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = Calendar2str(Val_str(arg[1]), Val_Date(arg[0]));
}

static void f_Date_print (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = PrintCalendar(Val_io(arg[1]),
		Val_str(arg[2]), Val_Date(arg[0]));
}

static void f_Date2int (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = Val_Date(arg[0]);
}

static void f_Date2dbl (EfiFunc *func, void *rval, void **arg)
{
	CalInfo *x = Calendar(Val_Date(arg[0]), NULL);
	double width = LeapYear(x->year) ? 366. : 365.;
	Val_double(rval) = x->year + (x->yday - 0.5) / width;
}

static void f_Date(EfiFunc *func, void *rval, void **arg)
{
	Val_Date(rval) = CalendarIndex(Val_int(arg[0]), Val_int(arg[1]),
		Val_int(arg[2]));
}

static void f_StdDate(EfiFunc *func, void *rval, void **arg)
{
	Val_Date(rval) = CalBaseStd(Val_int(arg[0]));
}

static void f_str2Date(EfiFunc *func, void *rval, void **arg)
{
	Val_Date(rval) = str2Calendar(Val_str(arg[0]), NULL, Val_bool(arg[1]));
}

static void f_today(EfiFunc *func, void *rval, void **arg)
{
	Val_Date(rval) = TodayIndex() + Val_int(arg[0]);
}

static void f_Date_dist (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = Val_Date(arg[0]) - Val_Date(arg[1]);
}

static void f_Date_cmp (EfiFunc *func, void *rval, void **arg)
{
	register int a = Val_Date(arg[0]);
	register int b = Val_Date(arg[1]);

	if	(a < b)	Val_int(rval) = -1;
	else if	(a > b)	Val_int(rval) = 1;
	else		Val_int(rval) = 0;
}

/*	Zeitfunktionen
*/

static void f_Time2str (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = Time2str(Time_fmt, TIME(arg[0]));
}

static void f_Time2dbl (EfiFunc *func, void *rval, void **arg)
{
	CalInfo *x = TimeCalendar(TIME(arg[0]), NULL);
	double width = LeapYear(x->year) ? 366. : 365.;
	Val_double(rval) = x->year + (x->yday - 1.) / width +
		(TIME(arg[0]).time + 0.5) / (24. * 30. * 30. * width);
}


static void f_Time_konv (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = Time2str(Val_str(arg[1]), TIME(arg[0]));
}

static void f_Time_print (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = PrintTime(Val_io(arg[1]),
		Val_str(arg[2]), TIME(arg[0]));
}

static void f_fprint_Time (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = PrintTime(Val_io(arg[0]),
		Time_fmt, TIME(arg[1]));
}

static void f_str2Time(EfiFunc *func, void *rval, void **arg)
{
	TIME(rval) = str2Time(Val_str(arg[0]), NULL, Val_bool(arg[1]));
}

static void f_localtime (EfiFunc *func, void *rval, void **arg)
{
	TIME(rval) = Time_offset(CurrentTime(), Val_int(arg[0]));
}

/*	Konvertierung
*/

static void f_Time2Date (EfiFunc *func, void *rval, void **arg)
{
	Val_Date(rval) = TIME(arg[0]).date;
}

static void f_Date2Time (EfiFunc *func, void *rval, void **arg)
{
	CalTimeIndex *tp = rval;
	tp->date = Val_Date(arg[0]);
	tp->time = 12 * 60 * 60;
}

static void f_Time_add (EfiFunc *func, void *rval, void **arg)
{
	TIME(arg[0]) = Time_offset(TIME(arg[0]), Val_int(arg[1]));
}

static void f_Time_sub (EfiFunc *func, void *rval, void **arg)
{
	TIME(arg[0]) = Time_offset(TIME(arg[0]), -Val_int(arg[1]));
}

static void f_Time_dist (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = Time_dist(TIME(arg[1]), TIME(arg[0]));
}

static void f_Time_cmp (EfiFunc *func, void *rval, void **arg)
{
	register CalTimeIndex *a = arg[0];
	register CalTimeIndex *b = arg[1];

	if	(a->date < b->date)	Val_int(rval) = -1;
	else if	(a->date > b->date)	Val_int(rval) = 1;
	else if	(a->time < b->time)	Val_int(rval) = -1;
	else if	(a->time > b->time)	Val_int(rval) = 1;
	else				Val_int(rval) = 0;
}

static void f_LeapYear (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = LeapYear(Val_int(arg[0]));
}

static void f_LeapYear_Date (EfiFunc *func, void *rval, void **arg)
{
	CalInfo *x = Calendar(Val_Date(arg[0]), NULL);
	Val_bool(rval) = LeapYear(x->year);
}

static void f_LeapYear_Time (EfiFunc *func, void *rval, void **arg)
{
	CalInfo *x = TimeCalendar(TIME(arg[0]), NULL);
	Val_bool(rval) = LeapYear(x->year);
}


/*	Funktionstabelle
*/

static EfiFuncDef fdef[] = {
	{ 0, &Type_Date, "Date (int t, int m, int j)", f_Date },
	{ 0, &Type_Date, "Date (int b1900)", f_StdDate },
	{ 0, &Type_Date, "Date (str s, bool end = false)", f_str2Date },
	{ FUNC_PROMOTION, &Type_int, "Date ()", f_Date2int },
	{ FUNC_RESTRICTED, &Type_str, "Date ()", f_Date2str },
	{ FUNC_RESTRICTED, &Type_double, "Date ()", f_Date2dbl },
	{ 0, &Type_str, "Date::konv (str fmt = NULL)", f_Date_konv },
	{ 0, &Type_int, "Date::print (IO out, str fmt = NULL)", f_Date_print },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, Date)", f_fprint_Date },
	{ FUNC_VIRTUAL, &Type_int, "operator- (Date, Date)", f_Date_dist },
	{ FUNC_VIRTUAL, &Type_int, "cmp (Date, Date)", f_Date_cmp },
	{ 0, &Type_Date, "today (int offset = 0)", f_today },

	{ 0, &Type_Time, "Time (str s, bool end = false)", f_str2Time },
	{ FUNC_RESTRICTED, &Type_str, "Time ()", f_Time2str },
	{ FUNC_RESTRICTED, &Type_double, "Time ()", f_Time2dbl },
	{ 0, &Type_str, "Time::konv (str fmt = NULL)", f_Time_konv },
	{ 0, &Type_int, "Time::print (IO out, str fmt = NULL)", f_Time_print },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, Time)", f_fprint_Time },
	{ 0, &Type_Time, "localtime (int offset = 0)", f_localtime },
	{ 0, &Type_Time, "& Time::operator+= & (int)", f_Time_add },
	{ 0, &Type_Time, "& Time::operator-= & (int)", f_Time_sub },

	{ 0, &Type_Time, "Date ()", f_Date2Time },
	{ FUNC_PROMOTION, &Type_Date, "Time ()", f_Time2Date },
	{ FUNC_VIRTUAL, &Type_int, "operator- (Time, Time)", f_Time_dist },
	{ FUNC_VIRTUAL, &Type_int, "cmp (Time, Time)", f_Time_cmp },
	{ FUNC_VIRTUAL, &Type_bool, "LeapYear (int year)", f_LeapYear },
	{ FUNC_VIRTUAL, &Type_bool, "LeapYear (Date date)", f_LeapYear_Date },
	{ FUNC_VIRTUAL, &Type_bool, "LeapYear (Time time)", f_LeapYear_Time },
	{ FUNC_VIRTUAL, &Type_list, "operator: (Date, Date)", RangeFunc },
	{ FUNC_VIRTUAL, &Type_list, "operator: (Date, Date, int)", RangeFunc },
	{ FUNC_VIRTUAL, &Type_list, "operator: (Time, Time)", RangeFunc },
	{ FUNC_VIRTUAL, &Type_list, "operator: (Time, Time, int)", RangeFunc },
};


/*	Initialisieren
*/

void CmdSetup_date(void)
{
	AddType(&Type_Date);
	AddVarDef(Type_Date.vtab, v_Date, tabsize(v_Date));
	AddEfiMember(Type_Date.vtab, m_Date, tabsize(m_Date));
	AddType(&Type_Time);
	AddVarDef(Type_Time.vtab, v_Time, tabsize(v_Time));
	AddEfiMember(Type_Time.vtab, m_Time, tabsize(m_Time));
	AddFuncDef(fdef, tabsize(fdef));
}
