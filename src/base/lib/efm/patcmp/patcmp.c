/*	Mustervergleich
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/patcmp.h>
#include <EFEU/mstring.h>


/*	Metazeichen
*/

#define	ANYCHAR(x)	(x == '?')
#define	ANYSTR(x)	(x == '*')
#define	ESCAPE(x)	(x == '\\')
#define	NEGATION(x)	(x == '!' || x == '^')


/*	Die Definition der Klammerung erfolgt durch eine
	Tabelle mit Strings, die der Reihe nach den Klammerbeginn,
	den Klammertyp und das Klammerende definieren.
*/

#define	MFLAG	'm'	/* Multiplikator */
#define	PFLAG	'p'	/* Teilmuster */
#define	OFLAG	'o'	/* optionales Teilmuster */
#define	LFLAG	'l'	/* Listenvergleich */

static char *listdef[] = {
	"<m>", "{p}", ":o:", "[l]", "(l)",
};

#define	DEFDIM	sizeof(listdef)/sizeof(listdef[0])


/*	Hilfsfunktionen
*/

static char *checklist (int c);
static int dopatcmp (const char *pat, const char **ptr);
static void urange (const char *range, unsigned *a, unsigned *b);


/*	Rückgabecode von dopatcmp()
*/

#define	NOMATCH	0	/* Keine Übereinstimmung */
#define	INITIAL	1	/* Stringanfang entspricht dem Muster */
#define	MATCH	2	/* String entspricht dem Muster */


/*	Mustervergleich
*/

int patcmp(const char *p, const char *s, char **ptr)
{
	int flag;	/* Negationsflag für Muster */
	int val;	/* Rückgabewert von patcmp() */

	if	(ptr != NULL)	*ptr = NULL;

/*	Ein leeres Muster stimmt nur mit einem leeren String
	überein.
*/
	if	(p == NULL)
	{
		if	(ptr != NULL)	*ptr = (char *) s;

		return (s == NULL);
	}

/*	Test auf Negationszeichen am Beginn des Musters
*/
	flag = 1;

	while (NEGATION(*p))
	{
		flag = ! flag;
		p++;
	}

	if	(s == NULL)	return ! flag;

/*	Mustervergleich durchführen und Ergebnis prüfen
*/
	val = dopatcmp(p, &s);

	if	(ptr != NULL)
	{
		*ptr = (char *) s;
		return (val != NOMATCH ? flag : !flag);
	}
	else	return (val == MATCH ? flag : !flag);
}


/*	Hauptroutine
*/

static int dopatcmp(const char *pat, const char **ptr)
{
	const char *ldef;	/* Listendefinition */
	unsigned cmin;		/* Minimale Widerholungzahl */
	unsigned cmax;		/* Maximale Widerholungzahl */
	unsigned i;		/* Zähler */
	size_t n;		/* Hilfszähler */
	int val;		/* Hilfsvariable */

/*	Abarbeiten des Strings
*/
	cmin = cmax = 1;

	for (;;)
	{

	/*	beliebige Zeichenkette
	*/
		if	(ANYSTR(*pat))
		{
			const char *save;
			const char *p;

			do	pat++;
			while	(ANYSTR(*pat));
	
			if	(*pat == 0)
			{
				while (**ptr != 0)
					(*ptr)++;

				return MATCH;
			}
	
			save = *ptr;

			for (p = save; *p != 0; p++)
			{
				*ptr = p;
				val = dopatcmp(pat, ptr);
	
				if	(val == MATCH)
					return val;
			}
	
			*ptr = save;
			break;
		}
	
	/*	Klammerung
	*/
		if	((ldef = checklist(*pat)) != NULL)
		{
			char *s;

			for (pat++, n = 0; pat[n] != ldef[2]; n++)
			{
				if (ESCAPE(pat[n]))	n++;
				if (pat[n] == 0)	break;
			}

			s = mstrncpy(pat, n);
			pat += (pat[n] == 0 ? n : n + 1);
			val = 1;

			switch (ldef[1])
			{
			case LFLAG:

				for (i = 0; i < cmax; i++)
				{
					if (!listcmp(s, **ptr))
						break;

					(*ptr)++;
				}

				val = (i >= cmin);
				cmin = cmax = 1;
				break;

			case PFLAG:

				for (i = 0; i < cmax; i++)
				{
					char *x;

					if	(!patcmp(s, *ptr, &x))
					{
						break;
					}

					*ptr = x;
				}

				val = (i >= cmin);
				cmin = cmax = 1;
				break;
				
			case OFLAG:

				(void) dopatcmp(s, ptr);
				break;


			case MFLAG:

				urange(s, &cmin, &cmax);
				break;
			}

			memfree(s);

			if	(val)		continue;

			break;
		}

	/*	beliebiges Zeichen
	*/
		if	(ANYCHAR(*pat))
		{
			for (i = 0; i < cmax && **ptr != 0; i++)
				(*ptr)++;

			if	(i < cmin)	return NOMATCH;
		}

	/*	sonstiger Zeichenvergleich
	*/
		else
		{
			if	(ESCAPE(*pat))	pat++;

			if	(*pat == 0)
			{
				return (**ptr == 0 ? MATCH : INITIAL);
			}

			for (i = 0; i < cmax && **ptr == *pat; i++)
				(*ptr)++;

			if	(i < cmin)	return NOMATCH;
		}

		cmin = cmax = 1;
		pat++;
	}

	return NOMATCH;
}


/*	Test auf eine Klammerung: Die Funktion liefert
	entweder NULL oder den String mit der Klammerdefinition.
*/

static char *checklist(int c)
{
	int i;

	for (i = 0; i < DEFDIM; i++)
	{
		if	(listdef[i][0] == (char) c)
		{
			return (char *) listdef[i];
		}
	}

	return NULL;
}


static void urange(const char *range, unsigned int *a, unsigned int *b)
{
	*a = 0;
	*b = ~0;

	if	(range == NULL || *range == 0)
	{
		return;
	}

	if	(*range != '-')
	{
		char *p;

		*a = (unsigned) strtol(range, &p, 0);
		range = p;
	}

	if	(*range == 0)
	{
		*b = *a;
	}
	else if	(*range == '-' && *(++range) != 0)
	{
		*b = (unsigned) strtol(range, NULL, 0);
	}

	if	(*b < *a)
	{
		unsigned c;
		c = *a;
		*a = *b;
		*b = c;
	}
}
