/*
Hilfsprogramme

$Copyright (C) 1994 Erich Frühstück
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
#include <EFEU/datum.h>
#include <EFEU/calendar.h>
#include <time.h>

#define	RVINT	Val_int(rval)
#define	RVSTR	Val_str(rval)

#define	STR(n)		Val_str(arg[n])
#define	INT(n)		Val_int(arg[n])
#define	LONG(n)		Val_long(arg[n])
#define	DOUBLE(n)	Val_double(arg[n])
#define	TIME(x)		((Time_t *) (x))[0]


/*	Datums- und Zeittype
*/

Type_t Type_Date = SIMPLE_TYPE("Date", int, &Type_int);
Type_t Type_Time = SIMPLE_TYPE("Time", Time_t, NULL);

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

static Obj_t *cal2obj (Calendar_t *cal, int type)
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

static Obj_t *base1900 (const Var_t *var, const Obj_t *obj)
{
	return int2Obj(obj ? CalBase1900(Val_Date(obj->data)) : 0);
}

static Obj_t *Date_member (const Var_t *var, const Obj_t *obj)
{
	return cal2obj(obj ? Calendar(Val_Date(obj->data), NULL) : NULL,
		(int) (size_t) var->par);
}

static VarDef_t v_Date[] = {
	{ "fmt", &Type_str, &Date_fmt },
};

static MemberDef_t m_Date[] = {
	{ "day", &Type_int, Date_member, (void *) DM_DAY },
	{ "month", &Type_int, Date_member, (void *) DM_MON },
	{ "year", &Type_int, Date_member, (void *) DM_YEAR },
	{ "wday", &Type_int, Date_member, (void *) DM_WDAY },
	{ "yday", &Type_int, Date_member, (void *) DM_YDAY },
	{ "cal1900", &Type_int, base1900, NULL },
};

static Obj_t *Time_member (const Var_t *var, const Obj_t *obj)
{
	return cal2obj(obj ? TimeCalendar(TIME(obj->data), NULL) : NULL,
		(int) (size_t) var->par);
}

static VarDef_t v_Time[] = {
	{ "fmt", &Type_str, &Date_fmt },
};

static MemberDef_t m_Time[] = {
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

static void f_fprint_Date (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = PrintCalendar(Val_io(arg[0]),
		Date_fmt, Val_Date(arg[1]));
}

static void f_Date2str (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = Calendar2str(Date_fmt, Val_Date(arg[0]));
}

static void f_Date_konv (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = Calendar2str(Val_str(arg[1]), Val_Date(arg[0]));
}

static void f_Date_print (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = PrintCalendar(Val_io(arg[1]),
		Val_str(arg[2]), Val_Date(arg[0]));
}

static void f_Date2int (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = Val_Date(arg[0]);
}

static void f_Date2dbl (Func_t *func, void *rval, void **arg)
{
	Calendar_t *x = Calendar(Val_Date(arg[0]), NULL);
	double width = LeapYear(x->year) ? 366. : 365.;
	Val_double(rval) = x->year + (x->yday - 0.5) / width;
}

static void f_Date(Func_t *func, void *rval, void **arg)
{
	Val_Date(rval) = CalendarIndex(Val_int(arg[0]), Val_int(arg[1]),
		Val_int(arg[2]));
}

static void f_StdDate(Func_t *func, void *rval, void **arg)
{
	Val_Date(rval) = CalBaseStd(Val_int(arg[0]));
}

static void f_str2Date(Func_t *func, void *rval, void **arg)
{
	Val_Date(rval) = str2Calendar(Val_str(arg[0]), NULL, Val_bool(arg[1]));
}

static void f_today(Func_t *func, void *rval, void **arg)
{
	Val_Date(rval) = TodayIndex() + Val_int(arg[0]);
}

static void f_Date_dist (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = Val_Date(arg[0]) - Val_Date(arg[1]);
}

static void f_Date_cmp (Func_t *func, void *rval, void **arg)
{
	register int a = Val_Date(arg[0]);
	register int b = Val_Date(arg[1]);

	if	(a < b)	Val_int(rval) = -1;
	else if	(a > b)	Val_int(rval) = 1;
	else		Val_int(rval) = 0;
}

/*	Zeitfunktionen
*/

static void f_Time2str (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = Time2str(Time_fmt, TIME(arg[0]));
}

static void f_Time2dbl (Func_t *func, void *rval, void **arg)
{
	Calendar_t *x = TimeCalendar(TIME(arg[0]), NULL);
	double width = LeapYear(x->year) ? 366. : 365.;
	Val_double(rval) = x->year + (x->yday - 1.) / width +
		(TIME(arg[0]).time + 0.5) / (24. * 30. * 30. * width);
}


static void f_Time_konv (Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = Time2str(Val_str(arg[1]), TIME(arg[0]));
}

static void f_Time_print (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = PrintTime(Val_io(arg[1]),
		Val_str(arg[2]), TIME(arg[0]));
}

static void f_fprint_Time (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = PrintTime(Val_io(arg[0]),
		Time_fmt, TIME(arg[1]));
}

static void f_str2Time(Func_t *func, void *rval, void **arg)
{
	TIME(rval) = str2Time(Val_str(arg[0]), NULL, Val_bool(arg[1]));
}

static void f_localtime (Func_t *func, void *rval, void **arg)
{
	TIME(rval) = Time_offset(CurrentTime(), Val_int(arg[0]));
}

/*	Konvertierung
*/

static void f_Time2Date (Func_t *func, void *rval, void **arg)
{
	Val_Date(rval) = TIME(arg[0]).date;
}

static void f_Date2Time (Func_t *func, void *rval, void **arg)
{
	Time_t *tp = rval;
	tp->date = Val_Date(arg[0]);
	tp->time = 12 * 60 * 60;
}

static void f_Time_add (Func_t *func, void *rval, void **arg)
{
	TIME(arg[0]) = Time_offset(TIME(arg[0]), Val_int(arg[1]));
}

static void f_Time_sub (Func_t *func, void *rval, void **arg)
{
	TIME(arg[0]) = Time_offset(TIME(arg[0]), -Val_int(arg[1]));
}

static void f_Time_dist (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = Time_dist(TIME(arg[1]), TIME(arg[0]));
}

static void f_Time_cmp (Func_t *func, void *rval, void **arg)
{
	register Time_t *a = arg[0];
	register Time_t *b = arg[1];

	if	(a->date < b->date)	Val_int(rval) = -1;
	else if	(a->date > b->date)	Val_int(rval) = 1;
	else if	(a->time < b->time)	Val_int(rval) = -1;
	else if	(a->time > b->time)	Val_int(rval) = 1;
	else				Val_int(rval) = 0;
}

static void f_LeapYear (Func_t *func, void *rval, void **arg)
{
	Val_bool(rval) = LeapYear(Val_int(arg[0]));
}

static void f_LeapYear_Date (Func_t *func, void *rval, void **arg)
{
	Calendar_t *x = Calendar(Val_Date(arg[0]), NULL);
	Val_bool(rval) = LeapYear(x->year);
}

static void f_LeapYear_Time (Func_t *func, void *rval, void **arg)
{
	Calendar_t *x = TimeCalendar(TIME(arg[0]), NULL);
	Val_bool(rval) = LeapYear(x->year);
}


/*	Funktionstabelle
*/

static FuncDef_t fdef[] = {
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
};


/*	Initialisieren
*/

void CmdSetup_date(void)
{
	AddType(&Type_Date);
	AddVarDef(Type_Date.vtab, v_Date, tabsize(v_Date));
	AddMember(Type_Date.vtab, m_Date, tabsize(m_Date));
	AddType(&Type_Time);
	AddVarDef(Type_Time.vtab, v_Time, tabsize(v_Time));
	AddMember(Type_Time.vtab, m_Time, tabsize(m_Time));
	AddFuncDef(fdef, tabsize(fdef));
}
