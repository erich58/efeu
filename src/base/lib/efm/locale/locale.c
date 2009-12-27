/*	Sprachanpassung
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6
*/

#include <EFEU/locale.h>
#include <EFEU/memalloc.h>
#include <EFEU/vecbuf.h>

#define	NAME_US		"us"
#define	NAME_DE		"de"
#define	NAME_TEX	"TeX"
#define	NAME_DOC	"doc"
#define	NAME_ORA	"Oracle"

LCValue_t LCValue_US = { NAME_US, NULL, ".", "-", "+", " " };
LCValue_t LCValue_DE = { NAME_DE, ".", ",", "-", "+", " " };
LCValue_t LCValue_TEX = { NAME_TEX, ".", ",", "$-$", "$+$", " " };
LCValue_t LCValue_DOC = { NAME_DOC, ".", ",", "\\minus ", "\\plus ", " " };

LCDate_t LCDate_US = { NAME_US,
	{ "January", "February", "March", "April", "May", "June", "July",
		"August", "September", "October", "November", "December", },
	{ "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
		"Friday", "Saturday" },
};

LCDate_t LCDate_DE = { NAME_DE,
	{ "Jänner", "Februar", "März", "April", "Mai", "Juni", "Juli",
		"August", "September", "Oktober", "November", "Dezember", },
	{ "Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag",
		"Freitag", "Samstag" },
};

LCDate_t LCDate_ORA = { NAME_ORA,
	{ "Jannuar", "Februar", "März", "April", "Mai", "Juni", "Juli",
		"August", "September", "Oktober", "November", "Dezember", },
	{ "Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag",
		"Freitag", "Samstag" },
};

Locale_t Locale = { NULL, &LCValue_US, &LCValue_US, &LCDate_US };


/*	Lokale Stack
*/

typedef struct LCStack_s LCStack_t;

struct LCStack_s {
	LCStack_t *next;	/* Lokalestack */
	Locale_t locale;	/* Lokaledaten */
};

static ALLOCTAB(LCTAB, 8, sizeof(LCStack_t));

static LCStack_t *LocaleStack = NULL;

void PushLocale(void)
{
	LCStack_t *x = LocaleStack;
	LocaleStack = new_data(&LCTAB);
	LocaleStack->next = x;
	LocaleStack->locale = Locale;
}


void PopLocale(void)
{
	if	(LocaleStack)
	{
		LCStack_t *x = LocaleStack;
		LocaleStack = x->next;
		Locale = x->locale;
		del_data(&LCTAB, x);
	}
}


/*	Lokale - Datenbank
*/

static int lc_cmp(const Locale_t *a, const Locale_t *b)
{
	if	(a == b)		return 0;
	if	(a->name == b->name)	return 0;
	if	(a->name == NULL)	return -1;
	if	(b->name == NULL)	return 1;

	return strcmp(a->name, b->name);
}

static VECBUF(LocaleTab, 8, sizeof(Locale_t));

static Locale_t DefTab[] = {
	{ NULL,	&LCValue_US, &LCValue_US, &LCDate_DE },
	{ NAME_US, &LCValue_US, &LCValue_US, &LCDate_US },
	{ NAME_DE, &LCValue_DE, &LCValue_DE, &LCDate_DE },
	{ NAME_TEX, &LCValue_US, &LCValue_TEX, &LCDate_DE },
	{ NAME_DOC, &LCValue_US, &LCValue_DOC, &LCDate_DE },
	{ NAME_ORA, &LCValue_DE, &LCValue_DE, &LCDate_ORA },
};


void AddLocale(Locale_t *data, size_t dim)
{
	for (; dim-- > 0; data++)
		vb_search(&LocaleTab, data, (comp_t) lc_cmp, VB_REPLACE);
}


int SetLocale(unsigned type, const char *name)
{
	Locale_t key, *x;

	if	(LocaleTab.used == 0)
		AddLocale(DefTab, sizeof(DefTab) / sizeof(DefTab[0]));

	key.name = (char *) name;
	x = vb_search(&LocaleTab, &key, (comp_t) lc_cmp, VB_SEARCH);

	if	(x == NULL)	return 0;

	if	(type & LOC_SCAN)
		Locale.scan = x->scan;

	if	(type & LOC_PRINT)
		Locale.print = x->print;

	if	(type & LOC_DATE)
		Locale.date = x->date;

	Locale.name = (type == LOC_ALL) ? x->name : NULL;
	return 1;
}

char *GetLocale(unsigned type)
{
	switch (type)
	{
	case LOC_SCAN:	return Locale.scan ? Locale.scan->name : NULL;
	case LOC_PRINT:	return Locale.print ? Locale.print->name : NULL;
	case LOC_DATE:	return Locale.date ? Locale.date->name : NULL;
	default:	return Locale.name;
	}
}
