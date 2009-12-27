/*
Kalenderrechnung

$Header <EFEU/$1>

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

#ifndef	EFEU_calendar_h
#define	EFEU_calendar_h	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>

#define	CALENDAR_OFFSET_1900 	693608	/* 31 Dez 1899 */

#define	SUNDAY_OFFSET(idx)	(((idx) + 1) % 7)
#define	MONDAY_OFFSET(idx)	((idx) % 7)

typedef struct {
	int day;	/* Tag */
	int month;	/* Monat */
	int year;	/* Jahr */
	int wday;	/* Wochentagsnummer, 0 = Sonntag) */
	int yday;	/* Tag des Jahres */
	int hour;	/* Stunde des Tages */
	int min;	/* Minute */
	int sec;	/* Sekunde */
} Calendar_t;

int LeapYear(int jahr);
int CalendarIndex (int tag, int monat, int jahr);
int TodayIndex (void);

Calendar_t *Calendar (int idx, Calendar_t *buf);

int PrintCalendar (io_t *io, const char *fmt, int idx);
char *Calendar2str (const char *fmt, int idx);
int str2Calendar (const char *str, char **endptr, int flag);


/*	Umrechnung in alte Kalenderbasis
*/

int CalBaseStd (int x);
int CalBase1900 (int x);

/*	Zeitstruktur
*/

typedef struct {
	unsigned date;	/* Datumsindex */
	unsigned time;	/* Zeitindex */
} Time_t;

Calendar_t *TimeCalendar (Time_t time, Calendar_t *buf);
int PrintTime (io_t *io, const char *fmt, Time_t time);
char *Time2str (const char *fmt, Time_t idx);
Time_t CurrentTime (void);
Time_t str2Time (const char *str, char **endptr, int flag);
Time_t Time_offset (Time_t time, int s);
int Time_dist (Time_t a, Time_t b);

#endif	/* EFEU/calendar.h */
