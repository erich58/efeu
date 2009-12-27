/*	Zeitreihenindex
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0

#	#Index
y	Jahr
q	Jahr:Quartal
w	Woche/Jahr
m	Monat.Jahr
d	Tag.Monat.Jahr
*/

#include <EFEU/object.h>
#include <EFEU/calendar.h>
#include <Math/TimeSeries.h>
#include <ctype.h>

#define	MONTAG(woche)	(7 * (woche) + 0)
#define	MITTWOCH(woche)	(7 * (woche) + 3)
#define	SONNTAG(woche)	(7 * (woche) + 6)

static int WeekIndex(int y, int w)
{
	return CalendarIndex(1, 1, y) / 7 + w - 1;
}

TimeIndex_t TimeIndex (int type, int value)
{
	TimeIndex_t idx;

	idx.type = type;
	idx.value = value;
	return idx;
}

TimeIndex_t str2TimeIndex (const char *str)
{
	TimeIndex_t idx;
	char *p;

	idx.type = TS_INDEX;
	idx.value = 0;

	if	(str == NULL)	return idx;
	
	while 	(isspace(*str))
		str++;

	if	(*str == '#')
	{
		idx.value = strtol(str, NULL, 10);
		return idx;
	}

	idx.value = strtol(str, &p, 10);
	idx.type = TS_YEAR;

	if	(p == NULL)
	{
		if	(idx.value < 100)	idx.value += 1900;
	}
	else if	(*p == '/')
	{
		int jahr = strtol(p + 1, NULL, 10);

		if	(jahr < 100)	jahr += 1900;

		idx.type = TS_WEEK;
		idx.value = WeekIndex(jahr, idx.value);
	}
	else if	(*p == ':')
	{
		if	(idx.value < 100)	idx.value += 1900;

		idx.type = TS_QUART;
		idx.value = 4 * idx.value + (strtol(p + 1, NULL, 10) - 1) % 4;
	}
	else if	(*p == '.')
	{
		int b = strtol(p + 1, &p, 10);

		if	(p && *p == '.')
		{
			int jahr = strtol(p + 1, &p, 10);

			idx.type = TS_DAY;
			idx.value = CalendarIndex(idx.value, b, jahr);
		}
		else
		{
			if	(b < 100)	b += 1900;

			idx.type = TS_MONTH;
			idx.value = 12 * b + (idx.value - 1) % 12;
		}
	}
	else if	(idx.value < 100)	idx.value += 1900;

	return idx;
}


char *TimeIndex2str (TimeIndex_t idx, int offset)
{
	Calendar_t cal;
	int week;

	idx.value += offset;

	switch (idx.type)
	{
	case TS_DAY:
		Calendar(idx.value, &cal);
		return msprintf("%d.%d.%d", cal.day, cal.month, cal.year);
	case TS_WEEK:
		Calendar(MITTWOCH(idx.value), &cal);
		week = idx.value - WeekIndex(cal.year, 0);
		return msprintf("%d/%d", week, cal.year);
	case TS_MONTH:
		return msprintf("%d.%d", 1 + idx.value % 12, idx.value / 12);
	case TS_QUART:
		return msprintf("%d:%d", idx.value / 4, 1 + idx.value % 4);
	case TS_YEAR:
		return msprintf("%d", idx.value);
	default:
		return msprintf("#%d", idx.value);
	}
}

static double dat2dbl(unsigned idx, int pos)
{
	Calendar_t cal;
	double width;

	Calendar(idx, &cal);
	width = LeapYear(cal.year) ? 366. : 365.;
	return cal.year + (cal.yday + 0.5 * pos - 1.0) / width;
}

static double week2dbl(unsigned idx, int pos)
{
	if	(pos == 0)	idx = MONTAG(idx);
	else if	(pos == 1)	idx = MITTWOCH(idx);
	else			idx = SONNTAG(idx);

	return dat2dbl(idx, pos);
}

double TimeIndex2dbl (TimeIndex_t idx, int offset, int pos)
{
	idx.value += offset;

	switch (idx.type)
	{
	case TS_DAY:
		return dat2dbl(idx.value, pos);
	case TS_WEEK:
		return week2dbl(idx.value, pos);
	case TS_MONTH:
		return (idx.value + 0.5 * pos) / 12.;
	case TS_QUART:
		return (idx.value + 0.5 * pos) / 4.;
	case TS_YEAR:
		return idx.value + 0.5 * pos;
	default:
		return idx.value + 0.5 *pos;
	}
}

static int mlength(unsigned first, unsigned width)
{
	int month = 1 + first % 12;
	int year = first / 12;

	first = CalendarIndex(1, month, year);

	month += width;

	if	(month > 12)
	{
		month -= 12;
		year++;
	}

	return CalendarIndex(1, month, year) - first;
}

int TimeIndexLength (TimeIndex_t idx)
{
	switch (idx.type)
	{
	case TS_DAY:	return 1;
	case TS_WEEK:	return 7;
	case TS_MONTH:	return mlength(idx.value, 1);
	case TS_QUART:	return mlength(3 * idx.value, 3);
	case TS_YEAR:	return mlength(12 * idx.value, 12);
	default:	return 1;
	}
}

int TimeIndexYear (TimeIndex_t idx)
{
	switch (idx.type)
	{
	case TS_DAY:	return Calendar(idx.value, NULL)->year;
	case TS_WEEK:	return Calendar(MITTWOCH(idx.value), NULL)->year;
	case TS_MONTH:	return idx.value / 12;
	case TS_QUART:	return idx.value / 4;
	case TS_YEAR:	return idx.value;
	default:	return 0;
	}
}

int TimeIndexMonth (TimeIndex_t idx)
{
	switch (idx.type)
	{
	case TS_DAY:	return Calendar(idx.value, NULL)->month;
	case TS_WEEK:	return Calendar(MITTWOCH(idx.value), NULL)->month;
	case TS_MONTH:	return 1 + idx.value % 12;
	case TS_QUART:	return 2 + idx.value % 4;
	default:	return 0;
	}
}

#define	 month2day(x)	CalendarIndex(1, 1 + (x) % 12, (x) / 12)

int TimeIndexFloor (TimeIndex_t idx)
{
	switch (idx.type)
	{
	case TS_DAY:	return idx.value;
	case TS_WEEK:	return MONTAG(idx.value);
	case TS_MONTH:	return month2day(idx.value);
	case TS_QUART:	return month2day(3 * idx.value);
	case TS_YEAR:	return CalendarIndex(1, 1, idx.value);
	default:	return idx.value;
	}
}

int TimeIndexCeil (TimeIndex_t idx)
{
	switch (idx.type)
	{
	case TS_DAY:	return idx.value;
	case TS_WEEK:	return SONNTAG(idx.value);
	case TS_MONTH:	return month2day(idx.value + 1) - 1;
	case TS_QUART:	return month2day(3 * idx.value + 3) - 1;
	case TS_YEAR:	return CalendarIndex(31, 12, idx.value);
	default:	return idx.value;
	}
}

int DiffTimeIndex (TimeIndex_t a, TimeIndex_t b)
{
	if	(a.type != b.type)
	{
		reg_fmt(1, "%#c", a.type);
		reg_fmt(2, "%#c", b.type);
		errmsg(MSG_TS, 1);
		b = TimeIndexKonv(b, a.type, 0);
	}

	return (int) b.value - (int) a.value;
}


int TimeIndexType(const char *name)
{
	if	(name == NULL)	return 0;

	switch (*name)
	{
	case 'j':
	case 'J':
	case 'y':
	case 'Y':
		return TS_YEAR;
	case 'q':
	case 'Q':
		return TS_QUART;
	case 'm':
	case 'M':
		return TS_MONTH;
	case 'w':
	case 'W':
		return TS_WEEK;
	case 't':
	case 'T':
	case 'd':
	case 'D':
		return TS_DAY;
	}

	reg_cpy(1, name);
	errmsg(MSG_TS, 2);
	return 0;
}
