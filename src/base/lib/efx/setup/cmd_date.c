/*	Hilfsprogramme
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
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

static Var_t var_Date[] = {
	{ "fmt", &Type_str, &Date_fmt, 0, 0, NULL, NULL },
	{ "day", &Type_int, NULL, 0, 0, Date_member, (void *) DM_DAY },
	{ "month", &Type_int, NULL, 0, 0, Date_member, (void *) DM_MON },
	{ "year", &Type_int, NULL, 0, 0, Date_member, (void *) DM_YEAR },
	{ "wday", &Type_int, NULL, 0, 0, Date_member, (void *) DM_WDAY },
	{ "yday", &Type_int, NULL, 0, 0, Date_member, (void *) DM_YDAY },
	{ "cal1900", &Type_int, NULL, 0, 0, base1900, NULL },
};

static Obj_t *Time_member (const Var_t *var, const Obj_t *obj)
{
	return cal2obj(obj ? TimeCalendar(TIME(obj->data), NULL) : NULL,
		(int) (size_t) var->par);
}

static Var_t var_Time[] = {
	{ "fmt", &Type_str, &Time_fmt, 0, 0, NULL, NULL },
	{ "day", &Type_int, NULL, 0, 0, Time_member, (void *) DM_DAY },
	{ "month", &Type_int, NULL, 0, 0, Time_member, (void *) DM_MON },
	{ "year", &Type_int, NULL, 0, 0, Time_member, (void *) DM_YEAR },
	{ "wday", &Type_int, NULL, 0, 0, Time_member, (void *) DM_WDAY },
	{ "yday", &Type_int, NULL, 0, 0, Time_member, (void *) DM_YDAY },
	{ "hour", &Type_int, NULL, 0, 0, Time_member, (void *) DM_HOUR },
	{ "min", &Type_int, NULL, 0, 0, Time_member, (void *) DM_MIN },
	{ "sec", &Type_int, NULL, 0, 0, Time_member, (void *) DM_SEC },
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
	Val_Date(rval) = str2Calendar(Val_str(arg[0]), NULL);
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
	TIME(rval) = str2Time(Val_str(arg[0]), NULL);
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
	Val_ptr(rval) = arg[0];
}

static void f_Time_sub (Func_t *func, void *rval, void **arg)
{
	TIME(arg[0]) = Time_offset(TIME(arg[0]), -Val_int(arg[1]));
	Val_ptr(rval) = arg[0];
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


/*	Funktionstabelle
*/

static FuncDef_t fdef[] = {
	{ 0, &Type_Date, "Date (int t, int m, int j)", f_Date },
	{ 0, &Type_Date, "Date (int b1900)", f_StdDate },
	{ 0, &Type_Date, "Date (str s)", f_str2Date },
	{ FUNC_PROMOTION, &Type_int, "Date ()", f_Date2int },
	{ FUNC_RESTRICTED, &Type_str, "Date ()", f_Date2str },
	{ FUNC_RESTRICTED, &Type_double, "Date ()", f_Date2dbl },
	{ 0, &Type_str, "Date::konv (str fmt = NULL)", f_Date_konv },
	{ 0, &Type_int, "Date::print (IO out, str fmt = NULL)", f_Date_print },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, Date)", f_fprint_Date },
	{ FUNC_VIRTUAL, &Type_int, "operator- (Date, Date)", f_Date_dist },
	{ FUNC_VIRTUAL, &Type_int, "cmp (Date, Date)", f_Date_cmp },
	{ 0, &Type_Date, "today (int offset = 0)", f_today },

	{ 0, &Type_Time, "Time (str s)", f_str2Time },
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
};


/*	Initialisieren
*/

void CmdSetup_date(void)
{
	AddType(&Type_Date);
	AddVar(Type_Date.vtab, var_Date, tabsize(var_Date));
	AddType(&Type_Time);
	AddVar(Type_Time.vtab, var_Time, tabsize(var_Time));
	AddFuncDef(fdef, tabsize(fdef));
}
