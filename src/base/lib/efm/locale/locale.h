/*	Sprachanpassung
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6

$Header	<EFEU/$1>
*/

#ifndef	EFEU_LOCALE_H
#define	EFEU_LOCALE_H	1

#include <EFEU/config.h>

/*	Locale - Definitionen
*/

typedef struct {
	char *name;
	char *thousands_sep;
	char *decimal_point;
	char *negative_sign;
	char *positive_sign;
	char *zero_sign;
} LCValue_t;

extern LCValue_t LCValue_US;
extern LCValue_t LCValue_DE;
extern LCValue_t LCValue_TEX;

typedef struct {
	char *name;
	char *month[12];
	char *weekday[7];
} LCDate_t;

extern LCDate_t LCDate_US;
extern LCDate_t LCDate_DE;

typedef struct {
	char *name;
	LCValue_t *scan;
	LCValue_t *print;
	LCDate_t *date;
} Locale_t;

extern Locale_t Locale;


/*	Lokale Stack
*/

void PushLocale (void);
void PopLocale (void);


/*	Lokale - Datenbank
*/

#define	LOC_SCAN	0x1
#define	LOC_PRINT	0x2
#define	LOC_DATE	0x4
#define	LOC_ALL		0xff

void AddLocale (Locale_t *data, size_t dim);
int SetLocale (unsigned flags, const char *name);
char *GetLocale (unsigned flags);

#endif	/* EFEU/locale.h */
