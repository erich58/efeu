/*	Datumskonvertierungen
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_DATE_H
#define	EFEU_DATE_H	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>

typedef struct {
	int day;
	int month;
	int year;
	int wday;	/* Wochentagnummer, 0 = Sonntag)
	int dofy;	/* Tag des Jahres */
	int wofy;	/* Woche des Jahres */
} Date_t;

unsigned Today1900 (void);
unsigned Date1900 (int tag, int monat, int jahr);
Date_t *KonvDate1900 (unsigned date, Date_t *buf);

PrintDate (io_t *io, const char *fmt, Date_t *date);
char *Date2str (io_t *io, const char *fmt, Date_t *date);

#endif	/* EFEU_DATE_H */
