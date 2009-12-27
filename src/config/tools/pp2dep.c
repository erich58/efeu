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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define	NAME_SIZE	1023	/* Maximallänge für Filenamen ohne Abschluß */
#define	BREAK_COL	70	/* Spaltenpostion für Fortsetzungszeile */

#define	ERR_MALLOC	"%s: malloc (%ld) failed.\n"
#define ERR_SIZE	"%s: truncating name (size > %ld).\n"
#define	FMT_USAGE	"usage: %s [-l] [-x name] target(s)\n"

char *ProgName = "";

/*	Dynamische Speicherverwaltung
*/

static void *xmalloc (size_t n)
{
	void *data = malloc(n);

	if	(data == NULL)
	{
		fprintf(stderr, ERR_MALLOC, ProgName, (unsigned long) n);
		exit(1);
	}

	return data;
}

static void xfree (void *data)
{
	if	(data)	free(data);
}


/*	Filenamen lesen
*/

static size_t load_name (char *buf, size_t size)
{
	size_t n;
	int c;

	while ((c = getchar()) != EOF)
	{
		if	(c == '#')
		{
			while ((c = getchar()) != '"')
				if (c == EOF) return 0;
			
			break;
		}

		while (c != '\n' && c != EOF)
			c = getchar();
	}

	if	(c == EOF)	return 0;

	n = 0;

	while ((c = getchar()) != EOF)
	{
		if (c == '"' || c == '\n') break;

		if	(n < size)
			buf[n++] = c;
		else	fprintf(stderr, ERR_SIZE, ProgName, (unsigned long) size);
	}

	while (c != '\n' && c != EOF)
		c = getchar();

	buf[n++] = 0;
	return n;
}


/*	Namensliste
*/

#define	TAB_BLK	512

static char **name_tab = NULL;
static size_t name_size = 0;
static size_t name_dim = 0;

static void add_name (const char *buf, size_t size)
{
	size_t i = name_dim;

	while (i-- > 0)
		if (strcmp(name_tab[i], buf) == 0) return;

	if	(name_dim + 1 >= name_size)
	{
		char **save = name_tab;
		name_size = TAB_BLK * (name_dim / TAB_BLK + 1);
		name_tab = xmalloc(name_size * sizeof(char *));
		memcpy(name_tab, save, name_dim * sizeof(char *));
		xfree(save);
	}

	name_tab[name_dim++] = memcpy(xmalloc(size), buf, size);
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

int main (int narg, char **arg)
{
	char *buf, *name, *delim;
	char *(*select)(char *name);
	size_t n, pos;

	ProgName = arg[0];
	select = all_files;

/*	Optionen abfragen
*/
	while ((n = getopt(narg, arg, "hlx:")) != EOF)
	{
		switch (n)
		{
		case 'h':
			fprintf(stderr, FMT_USAGE, ProgName);
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
	buf = xmalloc(NAME_SIZE + 1);

	while ((n = load_name(buf, NAME_SIZE)) != 0)
		add_name(buf, n);
	
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
