/*	Kalenderrechnung
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_CALENDAR_H
#define	EFEU_CALENDAR_H	1

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
int str2Calendar (const char *str, char **endptr);


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
Time_t str2Time (const char *str, char **endptr);
Time_t Time_offset (Time_t time, int s);
int Time_dist (Time_t a, Time_t b);

#endif	/* EFEU_CALENDAR_H */
