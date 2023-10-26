/*
:*:index of time series
:de:Zeitreihenindex

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
#include <EFEU/calendar.h>
#include <Math/TimeSeries.h>
#include <ctype.h>

/*
#	#Index
y	Jahr
q	Jahr@Quartal
m	Jahr-Monat
d	Jahr-Monat-Tag
h	Tag/Stunde
m	Stunde:Minute
s	Stunde:Minute:Sekunde
*/

TimeIndex tindex_create (int type, int value)
{
	TimeIndex idx;

	idx.type = type;
	idx.value = value;
	return idx;
}

static char *min_sec(TimeIndex *idx, char *p)
{
	idx->value *= 60;
	idx->value += strtol(p + 1, &p, 10);

	if	(*p == ':')
	{
		idx->value *= 60;
		idx->value += strtol(p + 1, &p, 10);
		idx->type = TS_SEC;
	}
	else
	{
		idx->type = TS_MIN;
	}

	return p;
}

TimeIndex str2TimeIndex (const char *str, char **ptr)
{
	TimeIndex idx;
	char *p;

	idx.type = TS_INDEX;
	idx.value = 0;

	if	(str == NULL)
	{
		if	(ptr)	*ptr = NULL;

		return idx;
	}
	
	while 	(isspace(*str))
		str++;

	if	(*str == '#')
	{
		idx.value = strtol(str + 1, ptr, 10);
		return idx;
	}

	idx.value = strtol(str, &p, 10);
	idx.type = TS_YEAR;

	if	(*p == 0)
	{
		idx.value = CenturyYear(idx.value);
	}
	else if	(*p == '/')
	{
		idx.value *= 24;
		idx.value += strtol(p + 1, &p, 10);
		idx.type = TS_HOUR;

		if	(*p == ':')
			p = min_sec(&idx, p);
	}
	else if	(*p == ':')
	{
		p = min_sec(&idx, p);
	}
	else if	(*p == '-')
	{
		int month = strtol(p + 1, &p, 10);
		idx.value = CenturyYear(idx.value);

		if	(*p == '-')
		{
			int day = strtol(p + 1, &p, 10);
			idx.type = TS_DAY;
			idx.value = CalendarIndex(day, month, idx.value);
		}
		else
		{
			idx.type = TS_MONTH;
			idx.value = 12 * idx.value + month - 1;
		}
	}
	else if	(*p == '@')
	{
		idx.value = CenturyYear(idx.value);
		idx.type = TS_QUART;
		idx.value = 4 * idx.value + strtol(p + 1, &p, 10) - 1;
	}
	else
	{
		idx.value = CenturyYear(idx.value);
	}

	if	(ptr)	*ptr = p;

	return idx;
}


int tindex_print (IO *io, TimeIndex idx, int offset)
{
	CalInfo cal;

	idx.value += offset;

	switch (idx.type)
	{
	case TS_DAY:
		Calendar(idx.value, &cal);
		return io_xprintf(io, "%04d-%02d-%02d",
			cal.year, cal.month, cal.day);
	case TS_MONTH:
		return io_xprintf(io, "%04d-%02d",
			idx.value / 12, 1 + idx.value % 12);
	case TS_QUART:
		return io_xprintf(io, "%04d@%d",
			idx.value / 4, 1 + idx.value % 4);
	case TS_YEAR:
		return io_xprintf(io, "%04d", idx.value);
	case TS_HOUR:
		return io_xprintf(io, "%d/%02d",
			idx.value / 24, idx.value % 24);
	case TS_MIN:
		return io_xprintf(io, "%02d:%02d",
			idx.value / 60, idx.value % 60);
	case TS_SEC:
		return io_xprintf(io, "%02d:%02d:%02d",
			idx.value / (60 * 60), (idx.value / 60) % 60,
			idx.value % 60);
	default:
		return io_xprintf(io, "#%d", idx.value);
	}
}

char *TimeIndex2str (TimeIndex idx, int offset)
{
	StrBuf *buf = sb_acquire();
	IO *io = io_strbuf(buf);

	tindex_print(io, idx, offset);
	io_close(io);
	return sb_cpyrelease(buf);
}

static double dat2dbl (unsigned idx, int pos)
{
	CalInfo cal;
	double width;

	Calendar(idx, &cal);
	width = LeapYear(cal.year) ? 366. : 365.;
	return cal.year + (cal.yday + 0.5 * pos - 1.0) / width;
}

double TimeIndex2dbl (TimeIndex idx, int offset, int pos)
{
	idx.value += offset;

	switch (idx.type)
	{
	case TS_DAY:
		return dat2dbl(idx.value, pos);
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

static int mlength (unsigned first, unsigned width)
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

int tindex_length (TimeIndex idx)
{
	switch (idx.type)
	{
	case TS_DAY:	return 1;
	case TS_MONTH:	return mlength(idx.value, 1);
	case TS_QUART:	return mlength(3 * idx.value, 3);
	case TS_YEAR:	return mlength(12 * idx.value, 12);
	case TS_HOUR:	return 3600;
	case TS_MIN:	return 60;
	default:	return 1;
	}
}

int tindex_year (TimeIndex idx)
{
	switch (idx.type)
	{
	case TS_DAY:	return Calendar(idx.value, NULL)->year;
	case TS_MONTH:	return idx.value / 12;
	case TS_QUART:	return idx.value / 4;
	case TS_YEAR:	return idx.value;
	default:	return 0;
	}
}

int tindex_quart (TimeIndex idx)
{
	int mon;

	switch (idx.type)
	{
	case TS_DAY:	mon = Calendar(idx.value, NULL)->month; break;
	case TS_MONTH:	mon = 1 + idx.value % 12; break;
	case TS_QUART:	return 1 + (idx.value % 4);
	default:	return 0;
	}

	return 1 + (mon - 1) / 3;
}

int tindex_month (TimeIndex idx)
{
	switch (idx.type)
	{
	case TS_DAY:	return Calendar(idx.value, NULL)->month;
	case TS_MONTH:	return 1 + idx.value % 12;
	case TS_QUART:	return 2 + 3 * (idx.value % 4);
	default:	return 0;
	}
}

#define	 month2day(x)	CalendarIndex(1, 1 + (x) % 12, (x) / 12)

int tindex_floor (TimeIndex idx)
{
	switch (idx.type)
	{
	case TS_DAY:	return idx.value;
	case TS_MONTH:	return month2day(idx.value);
	case TS_QUART:	return month2day(3 * idx.value);
	case TS_YEAR:	return CalendarIndex(1, 1, idx.value);
	default:	return idx.value;
	}
}

int tindex_ceil (TimeIndex idx)
{
	switch (idx.type)
	{
	case TS_DAY:	return idx.value;
	case TS_MONTH:	return month2day(idx.value + 1) - 1;
	case TS_QUART:	return month2day(3 * idx.value + 3) - 1;
	case TS_YEAR:	return CalendarIndex(31, 12, idx.value);
	default:	return idx.value;
	}
}

int tindex_diff (TimeIndex a, TimeIndex b)
{
	if	(a.type != b.type)
	{
		log_note(NULL, "[TimeSeries:1]", "cc", a.type, b.type);
		return 0;
	}

	return (int) b.value - (int) a.value;
}


const char *tindex_name(int type)
{
	switch (type)
	{
	case TS_YEAR:	return "year";
	case TS_QUART:	return "quart";
	case TS_MONTH:	return "month";
	case TS_DAY:	return "day";
	case TS_HOUR:	return "hour";
	case TS_MIN:	return "min";
	case TS_SEC:	return "sec";
	default:	return "index";
	}
}

int tindex_type (const char *name)
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
		switch (name[1])
		{
		case 'o':
		case 'O':
			return TS_MONTH;
		default:
			return TS_MIN;
		}
	case 't':
	case 'T':
	case 'd':
	case 'D':
		return TS_DAY;
	case 'h':
	case 'H':
		return TS_HOUR;
	case 's':
	case 'S':
		return TS_SEC;
	}

	log_note(NULL, "[TimeSeries:2]", "s", name);
	return 0;
}
