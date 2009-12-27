/*
:*:	create dependence list from cpp output
:de:	Abhängigkeitsregeln aus Präprozessorausgabe generieren

$Copyright (C) 1998 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define	NAME_SIZE	1023	/* Maximallänge für Filenamen ohne Abschluß */
#define	BREAK_COL	70	/* Spaltenpostion für Fortsetzungszeile */

#define	ERR_MALLOC	"malloc (%ld) failed.\n"
#define	FMT_USAGE	"usage: %s [-l] [-x name] target(s)\n"

/*
$pconfig
l|
	:*:list only local files
	:de:Nur lokale Dateien auflisten
x:pattern|
	:*:Exclude files which match <pattern>.
	:de:Dateien, die Muster <name> entsprechen, nicht auflisten
*target(s) |
	:*:list of targets
	:de:Liste der Generierungsziele
*/

char *ProgName = "";

static void usage (const char *arg)
{
	execlp("efeuman", "efeuman", "--", __FILE__, arg, NULL);
	fprintf(stderr, FMT_USAGE, ProgName);
	exit(arg ? 0 : 1);
}

static void error (const char *fmt, ...)
{
	va_list args;

	fprintf(stderr, "%s: ", ProgName);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(1);
}

/*
$Description
:*:
The command |$!| greps the names of included files from the output
of the C preprocessor and creates a dependend list for given targets.
:de:
Das Kommando |$!| filtert aus der Ausgabe des C--Preprozessors
die Namen von eingebundenen Dateien und stellt daraus eine Abhängigkeitsliste
für die angegebenen Ziele <target(s)> zusammen.

@arglist -i

:*:
The next lines show the typical use of |pp2dep| in a Makefiles:
:de:
Die typische Anwendung für |pp2dep| erfolgt in einem Makefile der
Form:

---- verbatim
file.o: file.c
	$$(CC) -c file.c

depend::
	$$(CC) -E -c file.c | pp2dep -l file.o >> Makefile
----

$SeeAlso
\mref{cc(1)}, \mref{make(1)}, \mref{mkmf(1)}, \mref{ppfilter(1)}.
*/


/*	Namensliste
*/

#define	TAB_BSIZE	512

static char **name_tab = NULL;
static size_t name_size = 0;
static size_t name_dim = 0;

static void add_name (const char *buf, size_t size)
{
	size_t i;
	char *p;

	for (i = name_dim; i-- > 0; )
		if (strcmp(name_tab[i], buf) == 0) return;

	if	(name_dim + 1 >= name_size)
	{
		char **save = name_tab;
		name_size += TAB_BSIZE;
		name_tab = realloc(name_tab, name_size * sizeof(char *));

		if	(!name_tab)
			error(ERR_MALLOC,
				(unsigned long) name_size * sizeof(char *));

		memcpy(name_tab, save, name_dim * sizeof(char *));
	}

	if	((p = malloc(size)) == NULL)
		error(ERR_MALLOC, (unsigned long) size);

	name_tab[name_dim++] = memcpy(p, buf, size);
}


/*	Filenamen lesen
*/

#define	BUF_BSIZE	250

static char *buf = NULL;
static size_t buf_size = 0;

static void eval_line (void)
{
	size_t n;
	int c;

	do	c = getchar();
	while	(c == ' ' || c == '\t');

	if	(c == 'p')	/* pragma ignorieren */
	{
		do	c = getchar();
		while	(c != '\n' && c != EOF);

		return;
	}

	while (c != '"')
	{
		if (c == '\n' || c == EOF) return;

		c = getchar();
	}

	n = 0;

	while ((c = getchar()) != EOF)
	{
		if	(c == '"' || c == '\n')
			break;

		if	(n >= buf_size)
		{
			buf_size += BUF_BSIZE;
			buf = realloc(buf, buf_size + 1);

			if	(!buf)
				error(ERR_MALLOC, (unsigned long) buf_size);
		}

		buf[n++] = c;
	}

	while (c != '\n' && c != EOF)
		c = getchar();

	buf[n++] = 0;

	if	(buf[0] != '<')
		add_name(buf, n);
}

/*	Hauptprogramm
*/

static char *name_mask = NULL;

static char *test_name (char *name, const char *base)
{
	char *p;

	if	(!(name && base))	return name;

	for (p = name; *p != 0; p++, base++)
	{
		if	(*base == '*')	return NULL;
		else if	(*base != *p)	return name;
	}

	switch (*base)
	{
	case  0:
	case '*':	return NULL;
	case '=':	return (char *) base + 1;
	default:	return name;
	}
}

static char *all_files (char *name)
{
	return test_name(name, name_mask);
}

static char *local_files (char *name)
{
	name = test_name(name, name_mask);
	name = test_name(name, "/usr/include/*");
	name = test_name(name, "/usr/lib/*");
	return name;
}


/*	Hauptprogramm
*/

extern int optind;
extern char *optarg;

int main (int narg, char **arg)
{
	char *name, *delim;
	char *(*select)(char *name);
	size_t n, pos;
	int c;

	ProgName = arg[0];
	select = all_files;

	if	(narg > 1)
	{
		if	(strncmp("--h", arg[1], 3) == 0)
			usage(arg[1]);
	}

/*	Optionen abfragen
*/
	while ((n = getopt(narg, arg, "?lx:")) != EOF)
	{
		switch (n)
		{
		case '?':
			usage(NULL);
			return 1;
		case 'l':
			select = local_files;
			break;
		case 'x':
			name_mask = optarg;
			break;
		}
	}

/*	Abhängigkeiten laden
*/
	arg += optind;
	narg -= optind;

	while ((c = getchar()) != EOF)
	{
		if	(c == '#')
		{
			eval_line();
		}
		else
		{
			while (c != '\n' && c != EOF)
				c = getchar();
		}
	}

/*	Bei fehlenden Zielnamen wird eine einfache Fileliste generiert
*/
	if	(narg == 0)
	{
		for (n = 0; n < name_dim; n++)
			if ((name = select(name_tab[n])))
				printf("%s\n", name);

		return 0;
	}

/*	Zielname(n) ausgeben
*/
	pos = 0;
	putchar('\n');
	delim = "";

	for (n = 0; n < narg; n++)
	{
		if	(!arg[n])	continue;

		pos += printf("%s%s", delim, arg[n]);
		delim = " ";
	}

	putchar(':');
	pos++;

/*	Abhängigkeiten ausgeben
*/
	for (n = 0; n < name_dim; n++)
	{
		if	(!(name = select(name_tab[n])))	continue;

		putchar(' ');
		pos++;

		if	(pos + strlen(name) >= BREAK_COL)
		{
			printf("\\\n ");
			pos = 1;
		}
			
		pos += printf("%s", name);
	}

	putchar('\n');
	exit(0);
}
