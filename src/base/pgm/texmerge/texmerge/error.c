/*	Fehlermeldungen für texmerge
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "texmerge.h"

typedef struct {
	int num;
	char *fmt;
} Error_t;

static Error_t ErrorTab[] = {
	{ 1, "Unbekannte Option '%c'." },
	{ 2, "Zuviele Argumente." },
	{ 3, "Speichersegment konnte nicht angefordert werden." },
	{ 4, "Mischbefehl \"%s\" ist bereits definiert." },
	{ 5, "Mischbefehl \"%s\" kann nicht gelöscht werden." },

	{ 10, "Konfigurationsfile \"%s\" nicht gefunden." },
	{ 11, "Eingabefile \"%s\" nicht lesbar." },
	{ 12, "Ausgabefile \"%s\" nicht lesbar." },
	{ 13, "Skriptfile \"%s\" nicht gefunden." },

	{ 21, "ACHTUNG: Umgebung \"%s\" hat unspezifisches Ende." },
	{ 22, "ACHTUNG: Umgebung \"%s\" endet mit \"%s\"." },
	{ 23, "Befehl \"%s\" ist nicht definiert." },
	{ 24, "Befehl \"%s\" Argumente nicht zugelassen." },
	{ 25, "ACHTUNG: Umgebung \"%s\", %d Argumente wurden ignoriert." },

	{ 31, "Zeichen '%c' anstelle einer Befehlskennung gelesen." },
	{ 32, "Zeichen '%c' kann nicht zurückgeschrieben werden." },

	{ 41, "Testblock nicht ausgeglichen." },
};

#define	TAB_DIM	(sizeof(ErrorTab) / sizeof(ErrorTab[0]))

static char *get_fmt(int num)
{
	int i;

	for (i = 0; i < TAB_DIM; i++)
		if (ErrorTab[i].num == num) return ErrorTab[i].fmt;

	return NULL;
}


void PrintError (int num, va_list args)
{
	char *fmt;

	if	((fmt = get_fmt(num)) != NULL)
	{
		vfprintf(stderr, fmt, args);
		putc('\n', stderr);
	}
	else	fprintf(stderr, "ERROR %d.\n", num);
}

void Error (int num, ...)
{
	va_list args;

	fprintf(stderr, "%s: ", ProgramName);
	va_start(args, num);
	PrintError(num, args);
	va_end(args);
}
