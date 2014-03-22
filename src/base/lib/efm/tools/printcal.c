/*
Datumsformatierung

$Copyright (C) 1995 Erich Frühstück
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

#include <EFEU/calendar.h>
#include <EFEU/locale.h>

#define	LOCALE	(Locale.date ? Locale.date : LCDate)

#define	MONAT(x)	LOCALE->month[x ? (x - 1) % 12 : 0]
#define	WTAG(x)		LOCALE->weekday[x % 7]

#define	FMT_DATE	"%Y-%m-%d"
#define	FMT_TIME	FMT_DATE " %H:%M:%S"

static int print_cal (IO *io, const char *fmt, CalInfo *cal);

typedef int (*PutVal) (IO *io, int w, int val);

static int pad_none(IO *io, int width, int val)
{
	return io_xprintf(io, "%d", val);
}

static int pad_zero (IO *io, int width, int val)
{
	return io_xprintf(io, "%0*d", width, val);
}

static int pad_space (IO *io, int width, int val)
{
	return io_xprintf(io, "%*d", width, val);
}


static int sun_week (CalInfo *cal)
{
	int x = (cal->yday - cal->wday - 1);
	return x <= 0 ?  0 : x / 7 + (x % 7 != 0);
}

static int mon_week (CalInfo *cal)
{
	int x = cal->wday ? cal->wday - 1 : 6;
	x = cal->yday - x - 1;
	return x <= 0 ?  0 : x / 7 + (x % 7 != 0);
}


static int print_key (IO *io, CalInfo *cal, int key, PutVal putval)
{
	switch (key)
	{
	case 'a':	return io_xprintf(io, "%2.2s", WTAG(cal->wday));
	case 'A':	return io_puts(WTAG(cal->wday), io);
	case 'h':
	case 'b':	return io_xprintf(io, "%3.3s", MONAT(cal->month));
	case 'B':	return io_puts(MONAT(cal->month), io);

	case 'd':	return putval(io, 2, cal->day);
	case 'e':	return io_xprintf(io, "%2d", cal->day);
	case 'j':	return putval(io, 3, cal->yday);
	case 'm':	return putval(io, 2, cal->month);
	case 'U':	return putval(io, 2, sun_week(cal));
	case 'V':	return putval(io, 2, CalInfo_week(cal));
	case 'W':	return putval(io, 2, mon_week(cal));
	case 'w':	return io_xprintf(io, "%d", cal->wday);
	case 'y':	return putval(io, 2, cal->year % 100);
	case 'Y':	return putval(io, 4, cal->year);

	case 'H':	return putval(io, 2, cal->hour);
	case 'I':	return putval(io, 2, 1 + (cal->hour + 11) % 12);
	case 'k':	return io_xprintf(io, "%d", cal->hour);
	case 'l':	return io_xprintf(io, "%d", 1 + (cal->hour + 11) % 12);
	case 'M':	return putval(io, 2, cal->min);
	case 'S':	return putval(io, 2, cal->sec);
	case 'p':	return io_puts(cal->hour >= 12 ? "PM" : "AM", io);
	case 'P':	return io_puts(cal->hour >= 12 ? "pm" : "am", io);
	case 'r':	return print_cal(io, "%I:%M:%S %p", cal);
	case 'R':	return print_cal(io, "%H:%M", cal);
	case 'T':	return print_cal(io, "%H:%M:%S", cal);
	case 'X':	return print_cal(io, "%H:%M:%S", cal);
	case 'F':	return print_cal(io, "%Y-%m-%d", cal);
	case 'D':	return print_cal(io, "%m/%d/%y", cal);

	case 'n':	return io_nputc('\n', io, 1);
	case 't':	return io_nputc('\t', io, 1);
	case '%':	return io_nputc('%', io, 1);

	case 'N':	return io_puts("000000000", io);

	/* Unfertig */
	case 'c':
	case 'C':
	case 'g':
	case 'u':
	case 'x':
	case 'G':
	case 'Z':
	case 's':	return io_puts("??", io);

	default:	return io_nputc(key, io, 1);
	}
}


static int print_cal (IO *io, const char *fmt, CalInfo *cal)
{
	PutVal putval;
	int n;

	if	(fmt == NULL)
	{
		return io_xprintf(io, "%2d.%2d.%04d", cal->day, cal->month,
			cal->year);
	}

	for (n = 0; *fmt != 0; fmt++)
	{
		if	(*fmt == '%')
		{
			putval = pad_zero;

			for (;;)
			{
				fmt++;

				if	(*fmt == 0)	return n;
				else if	(*fmt == '-')	putval = pad_none;
				else if	(*fmt == '_')	putval = pad_space;
				else	break;
			}

			n += print_key(io, cal, *fmt, putval);
		}
		else	n += io_nputc(*fmt, io, 1);
	}

	return n;
}

int PrintCalInfo (IO *io, const char *fmt, CalInfo *cal)
{
	if	(!cal)	cal = Calendar(0, NULL);

	return print_cal(io, fmt ? fmt : FMT_DATE, cal);
}

int PrintCalendar (IO *io, const char *fmt, int idx)
{
	return print_cal(io, fmt ? fmt : FMT_DATE, Calendar(idx, NULL));
}

int PrintTime (IO *io, const char *fmt, CalTimeIndex time)
{
	return print_cal(io, fmt ? fmt : FMT_TIME, TimeCalendar(time, NULL));
}

int Print_tm (IO *io, const char *fmt, struct tm *tm)
{
	return print_cal(io, fmt ? fmt : FMT_TIME, tmCalendar(tm, NULL));
}

/*	Konvertierung in Zeichenkette
*/

static char *cal2str (const char *fmt, CalInfo *cal)
{
	StrBuf *sb;
	IO *io;

	sb = sb_acquire();
	io = io_strbuf(sb);
	print_cal(io, fmt, cal);
	io_close(io);
	return sb_cpyrelease(sb);
}

char *Calendar2str (const char *fmt, int idx)
{
	return cal2str(fmt ? fmt : FMT_DATE, Calendar(idx, NULL));
}

char *Time2str (const char *fmt, CalTimeIndex time)
{
	return cal2str(fmt ? fmt : FMT_TIME, TimeCalendar(time, NULL));
}

char *tm2str (const char *fmt, struct tm *tm)
{
	return cal2str(fmt ? fmt : FMT_TIME, tmCalendar(tm, NULL));
}
