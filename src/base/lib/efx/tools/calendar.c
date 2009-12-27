/*
Kalenderrechnung

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

#include <EFEU/calendar.h>
#include <time.h>
#include <ctype.h>

#define	OFFSET_LIMIT	0xFFFF	/* Limit für Offset */
#define	SEC_OF_DAY	(24 * 60 * 60)

#define	YLIM_2000	50	/* Jahresgrenze für 2000 - Offset */
#define	YLIM_1900	100	/* Jahresgrenze für 1900 - Offset */


/*	leap year -- account for gregorian reformation in 1752
*/

#define	leap_year(yr) \
	((yr) <= 1752 ? !((yr) % 4) : \
	(!((yr) % 4) && ((yr) % 100)) || !((yr) % 400))

/*	number of centuries since 1700, not inclusive
*/

#define	centuries_since_1700(yr) \
	((yr) > 1700 ? (yr) / 100 - 17 : 0)

/*	number of centuries since 1700 whose modulo of 400 is 0
*/

#define	quad_centuries_since_1700(yr) \
	((yr) > 1600 ? ((yr) - 1600) / 400 : 0)

/*	number of leap years between year 1 and this year, not inclusive
*/

#define	leap_years_since_year_1(yr) \
	((yr) / 4 - centuries_since_1700(yr) + quad_centuries_since_1700(yr))

/*	Index des letzten Jahres
*/

#define	last_day_of_year(yr)	\
	((yr) * 365 + leap_years_since_year_1(yr))

/*	Schaltjahr
*/

int LeapYear (int year)
{
	if	(year < YLIM_2000)	year += 2000;
	if	(year < YLIM_1900)	year += 1900;

	return leap_year(year);
}


/*	Tag des Jahres
*/

static int DayInYear(int day, int month, int year)
{
	if	(month <= 2)	return day + 31 * (month - 1);

	if	(leap_year(year))	day++;

	return day + (int) (30.6 * month - 32.3);
}


/*	Kalenderindex
*/

int CalendarIndex (int day, int month, int year)
{
	if	(year < YLIM_2000)	year += 2000;
	if	(year < YLIM_1900)	year += 1900;

	return last_day_of_year(year - 1) + DayInYear(day, month, year);
}

/*	Aktuelles Datum
*/

int TodayIndex(void)
{
	time_t now;
	struct tm *x;

	time(&now);
	x = localtime(&now);
	return last_day_of_year(1900 + x->tm_year - 1) + x->tm_yday + 1;
}

Time_t CurrentTime (void)
{
	Time_t y;
	time_t now;
	struct tm *x;

	time(&now);
	x = localtime(&now);
	y.date = last_day_of_year(1900 + x->tm_year - 1) + x->tm_yday + 1;
	y.time = (x->tm_hour * 60 + x->tm_min) * 60 + x->tm_sec;
	return y;
}


/*	Kallenderstruktur setzen
*/

static Calendar_t cal_buf;

static Calendar_t *set_date (int idx, Calendar_t *buf)
{
	int n;
	int leap;

	if	(buf == NULL)	buf = &cal_buf;

	if	(idx == 0)
	{
		memset(buf, 0, sizeof(Calendar_t));
		return buf;
	}

	if	(idx <= OFFSET_LIMIT)
		idx += CALENDAR_OFFSET_1900;

	buf->year = (idx + 0.5) / 365.247;
	n = last_day_of_year(buf->year);

	if	(n >= idx)
	{
		buf->year--;
		n = last_day_of_year(buf->year);
	}

	buf->year++;
	buf->yday = idx - n;
	buf->wday = SUNDAY_OFFSET(idx);

	leap = leap_year(buf->year);
	n = buf->yday;

	if	(n <= 59 || (leap && n == 60))
	{
		buf->month = 1 + (n - 1) / 31;
		buf->day = n + 31 - 31 * buf->month;
	}
	else
	{
		if	(leap)	n--;

		buf->month = (int) ((n + 32.3) / 30.6);
		buf->day = (int) (n + 33.3 - 30.6 * buf->month);
	}

	buf->hour = 12;
	buf->min = 0;
	buf->sec = 0;
	return buf;
}

Calendar_t *Calendar (int idx, Calendar_t *buf)
{
	buf = set_date(idx, buf);
	buf->hour = 12;
	buf->min = 0;
	buf->sec = 0;
	return buf;
}

Calendar_t *TimeCalendar (Time_t time, Calendar_t *buf)
{
	buf = set_date(time.date, buf);
	buf->hour = time.time / 3600;
	buf->min = (time.time / 60) % 60;
	buf->sec = time.time % 60;
	return buf;
}


/*	Datumshilfsfunktionen
*/

int CalBaseStd(int x)
{
	return (x && x <= OFFSET_LIMIT) ? x + CALENDAR_OFFSET_1900 : x;
}

int CalBase1900(int x)
{
	if	(x <= OFFSET_LIMIT)	return x;
	if	(x <= CALENDAR_OFFSET_1900)	return 0;
	return x - CALENDAR_OFFSET_1900;
}

int str2Calendar (const char *str, char **endptr, int flag)
{
	int tag, monat, jahr;
	char *p;

	if	(str == NULL)	return 0;

	while (isspace(*str))
		str++;

	if	(!isdigit(*str))	return 0;

	jahr = strtol(str, &p, 10);
	monat = 1;
	tag = 1;

	if	(p && *p == '-' && isdigit(p[1]))
	{
		monat = strtol(p + 1, &p, 10);

		if	(p && *p == '-' && isdigit(p[1]))
		{
			tag = strtol(p + 1, &p, 10);
			flag = 0;
		}
	}
	else if	(p && *p == '.' && isdigit(p[1]))
	{
		monat = jahr;
		jahr = strtol(p + 1, &p, 10);

		if	(p && *p == '.' && isdigit(p[1]))
		{
			tag = monat;
			monat = jahr;
			jahr = strtol(p + 1, &p, 10);
			flag = 0;
		}
	}
	else if	(flag)
	{
		monat = 12;
	}

	if	(endptr)	*endptr = p;

	if	(jahr < YLIM_2000)	jahr += 2000;
	if	(jahr < YLIM_1900)	jahr += 1900;

	if	(flag)
	{
		if	(monat == 12)
		{
			jahr++;
			monat = 1;
		}
		else	monat++;

		return CalendarIndex(1, monat, jahr) - 1;
	}

	return CalendarIndex(tag, monat, jahr);
}

/*	Zeithilfsfunktionen
*/

Time_t str2Time (const char *str, char **endptr, int flag)
{
	int hour, min, sec;
	Time_t x;
	char *p;

	x.date = str2Calendar(str, &p, flag);
	hour = p ? strtol(p, &p, 10) : 12;

	if	(p && *p == ':')
	{
		min = strtol(p + 1, &p, 10);
		sec = p && *p == ':' ? strtol(p + 1, &p, 10) : 0;
	}
	else	min = sec = 0;

	if	(endptr)	*endptr = p;

	x.time = (hour * 60 + min) * 60 + sec;
	return x;
}

Time_t Time_offset (Time_t time, int s)
{
	int n = (s < 0) ? - (1 - s / SEC_OF_DAY) : s / SEC_OF_DAY;

	time.date += n;
	s -= n * SEC_OF_DAY;
	s += time.time;
	time.date += s / SEC_OF_DAY;
	time.time = s % SEC_OF_DAY;
	return time;
}

int Time_dist (Time_t t1, Time_t t2)
{
	int n, i1, i2;

	n = min(t1.date, t2.date);
	i1 = (t1.date - n) * SEC_OF_DAY + t1.time;
	i2 = (t2.date - n) * SEC_OF_DAY + t2.time;
	return i2 - i1;
}
