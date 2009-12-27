/*
:*:	evaluate project description files
:de: 	Auswerten von Projektbeschreibungsdateien

$Copyright (C) 2002 Erich Frühstück
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
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#define	USAGE	"usage: %s [-d] file(s)\n"
#define	NOREAD	"file %s not readable.\n"
#define	NOSPACE	"malloc(%lu) failed.\n"
#define	CIRCDEP	"circular dependence for %s.\n"
#define	NOTDEF	"dependence %s not defined.\n"
#define	LBLFMT	"%-11s %s\n"

/*
$pconfig
d|
	:*:create dependence list
	:de:Abhängigkeitsliste generieren
:file(s) |
	:*:project description file(s)
	:de:Projektbeschreibungsdatei(en)
*/

static char *callname = NULL;

static void usage (const char *arg)
{
	execlp("efeuman", "efeuman", "--", __FILE__, arg, NULL);
	fprintf(stderr, USAGE, callname);
	exit(arg ? 0 : 1);
}

/*
$Description
:*:The command |$1| is used by |efeubuild| to query the dependence information
for EFEU projects and to display the project label.
:de:Das Kommando |$1| wird von |efeubild| zur Bestimmung der
Abhängigkeiten zwischen einzelnen EFEU-Projekten und zur Ausgabe
der Projektbezeichnungen benutzt.

@arglist -i

$SeeAlso
efeubuild(1).
*/

static void error (const char *fmt, ...)
{
	va_list args;

	fprintf(stderr, "%s: ", callname);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(1);
}

static void *xmalloc (size_t n)
{
	void *p = malloc(n);

	if	(p == NULL)
		error(NOSPACE, (unsigned long) n);

	memset(p, 0, n);
	return p;
}

static void xfree (void *p)
{
	if	(p)	free(p);
}

static char *mstrncpy(const char *str, size_t n)
{
	if	(str)
	{
		char *tg = xmalloc(n + 1);
		strncpy(tg, str, n);
		tg[n] = 0;
		return tg;
	}
	else	return NULL;
}

static char *mstrcpy(const char *str)
{
	return str ? mstrncpy(str, strlen(str)) : NULL;
}

typedef struct NODE_S NODE;

struct NODE_S {
	char *name;
	NODE *next;
};

typedef struct {
	char *name;
	char *label;
	int depth;
	int lock;
	int flag;
	NODE *deplist;
} DESC;

#define	MAXLINE	1023

static char *getline (FILE *file)
{
	static char linebuf[MAXLINE + 1];
	int c, n;

	n = 0;

	while ((c = getc(file)) != EOF)
	{
		if	(c == '\n')
		{
			if	(n == 0)	continue;
			else			break;
		}
		else if	(n < MAXLINE)
		{
			linebuf[n++] = c;
		}
	}

	linebuf[n] = 0;
	return n ? linebuf : NULL;
}

static char *getkey (char **ptr)
{
	char *key, *p;

	key = *ptr;

	for (p = key; *p != 0; p++)
	{
		if	(*p == ' ' || *p == '\t')
		{
			*p = 0;

			do	p++;
			while	(*p == ' ' || *p == '\t');

			break;
		}
	}

	*ptr = p;
	return *key ? key : NULL;
}

static int iskey (const char *name, const char *arg)
{
	do
	{
		if	(*name != *arg)	return 0;

		name++;
		arg++;
	}
	while (*name && *arg && *arg != ':');

	return 1;
}

static char *Lang = "en";

static void desc_depend (DESC *desc, const char *name)
{
	if	(*name == '.')
	{
		;
	}
	else if	(*name == '*')
	{
		desc->flag = 1;
	}
	else
	{
		NODE *x = xmalloc(sizeof(NODE));
		x->name = mstrcpy(name);
		x->next = desc->deplist;
		desc->deplist = x;
	}
}

static void desc_load (DESC *desc, const char *name)
{
	FILE *file;
	char *p;
	char *key;
	int i, i0, i1;

	file = fopen(name, "r");

	if	(!file)
		error(NOREAD, name);

	for (i = i0 = i1 = 0; name[i] != 0; i++)
	{
		if	(name[i] == '/' && i > i1)
		{
			i0 = i1;
			i1 = i + 1;
		}
	}

	if	(i1 > i0)
	{
		desc->name = mstrncpy(name + i0, i1 - i0 - 1);
	}
	else	desc->name = mstrcpy(name);

	while ((p = getline(file)))
	{
		if	((key = getkey(&p)) == NULL)
		{
			continue;
		}
		else if	(iskey("Name", key))
		{
			xfree(desc->name);
			desc->name = mstrcpy(p);
		}
		else if	(iskey("Depends", key))
		{
			while ((key = getkey(&p)))
				desc_depend(desc, key);
		}
		else if	(*key != ':')
		{
			continue;
		}
		else if	(iskey(Lang, key + 1) || !desc->label)
		{
			xfree(desc->label);
			desc->label = mstrcpy(p);
		}
	}

	fclose(file);
}

static void desc_label (DESC *desc)
{
	printf(LBLFMT, desc->name, desc->label ? desc->label : "");
}

static void desc_show (DESC *desc, size_t n)
{
	NODE *p;
	int i;

	printf("%s", desc[n].name);

	if	(desc[n].flag)
	{
		for (i = 0; i < n; i++)
			if (!desc[i].flag)
				printf(" %s", desc[i].name);
	}

	for (p = desc[n].deplist; p != NULL; p = p->next)
		printf(" %s", p->name);

	putchar('\n');
}

static DESC *desc_get (const char *name, DESC *tab, size_t dim)
{
	for (; dim-- > 0; tab++)
		if (strcmp(name, tab->name) == 0) return tab;

	error(NOTDEF, name);
	return NULL;
}

static int desc_depth (DESC *desc, DESC *tab, size_t dim)
{
	if	(desc->depth)
	{
		;
	}
	else if	(desc->lock)
	{
		error(CIRCDEP, desc->name);
	}
	else if	(desc->deplist)
	{
		NODE *p;

		desc->lock = 1;

		for (p = desc->deplist; p != NULL; p = p->next)
		{
			int depth = desc_depth(desc_get(p->name, tab, dim),
				tab, dim) + 1;

			if	(desc->depth < depth)
				desc->depth = depth;
		}

		desc->lock = 0;
	}
	else	desc->depth = 1;

	return desc->depth;
}

static int cmp (const void *p1, const void *p2)
{
	const DESC *d1 = p1;
	const DESC *d2 = p2;

	if	(d1->flag != d2->flag)
	{
		if	(d2->flag)	return -1;
		if	(d1->flag)	return 1;
	}

	if	(d1->depth < d2->depth)	return -1;
	if	(d1->depth > d2->depth)	return 1;

	return strcmp(d1->name, d2->name);
}

int main (int argc, char **argv)
{
	int n, depend;
	DESC *desc;
	char *p;

	callname = argv[0];
	depend = 0;

	for (n = 1; n < argc; n++)
	{
		if	(argv[n][0] != '-')
		{
			break;
		}
		else if	(argv[n][1] == 'd')
		{
			depend = 1;
		}
		else	usage(argv[n]);
	}

	if	(argc == n)
		usage(NULL);

	if	((p = getenv("LANG")))
		Lang = mstrcpy(p);

	argv += n;
	argc -= n;
	desc = xmalloc(argc * sizeof(*desc));

	for (n = 0; n < argc; n++)
		desc_load(desc + n, argv[n]);
	
	if	(!depend)
	{
		for (n = 0; n < argc; n++)
			desc_label(desc + n);

		return 0;
	}

	for (n = 0; n < argc; n++)
	{
		desc_depth(desc + n,  desc, argc);
	}

	qsort(desc, argc, sizeof(*desc), cmp);

	for (n = 0; n < argc; n++)
		desc_show(desc, n);

	return 0;
}
