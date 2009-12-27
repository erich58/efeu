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

#include <EFEU/io.h>

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
} CalInfo;

extern int CenturyLimit;
int LeapYear (int jahr);
int CalendarIndex (int tag, int monat, int jahr);
int CalendarIndex1900 (int tag, int monat, int jahr);
int TodayIndex (void);

CalInfo *Calendar (int idx, CalInfo *buf);

int PrintCalendar (IO *io, const char *fmt, int idx);
char *Calendar2str (const char *fmt, int idx);
int str2Calendar (const char *str, char **endptr, int flag);
int Calendar_jdiff (unsigned t1, unsigned t2);

/*	Umrechnung in alte Kalenderbasis
*/

int CalBaseStd (int x);
int CalBase1900 (int x);

/*	Zeitstruktur
*/

typedef struct {
	unsigned date;	/* Datumsindex */
	unsigned time;	/* Zeitindex */
} CalTimeIndex;

CalInfo *TimeCalendar (CalTimeIndex time, CalInfo *buf);
int PrintTime (IO *io, const char *fmt, CalTimeIndex time);
char *Time2str (const char *fmt, CalTimeIndex idx);
CalTimeIndex CurrentTime (void);
CalTimeIndex str2Time (const char *str, char **endptr, int flag);
CalTimeIndex Time_offset (CalTimeIndex time, int s);
int Time_dist (CalTimeIndex a, CalTimeIndex b);

#endif	/* EFEU/calendar.h */
