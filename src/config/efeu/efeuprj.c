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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define	USAGE	"usage: %s [-d] file(s)\n"
#define	NOREAD	"file %s not readable.\n"
#define	CIRCDEP	"circular dependence for %s.\n"
#define	NOTDEF	"dependence %s not defined.\n"
#define	LBLFMT	"%-11s %s\n"

#define	NOSPACE	"sorry: malloc(%lu) failed\n"


static char *callname = NULL;

static void usage (const char *arg)
{
	execlp("efeuman", "efeuman", callname, arg, NULL);
	fprintf(stderr, USAGE, callname);
	exit(arg ? 0 : 1);
}


static void error (const char *fmt, ...)
{
	va_list args;

	fprintf(stderr, "%s: ", callname);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(1);
}

static void *xrealloc (void *data, size_t size)
{
	char *p;

	if	(!(p = realloc(data, size)))
		error(NOSPACE, (unsigned long) size);

	return p;
}

#define	xmalloc(size)	xrealloc(NULL, size)

static void xfree (void *data)
{
	if	(data)	free(data);
}

typedef struct NODE_S NODE;

struct NODE_S {
	char *name;
	NODE *next;
};

typedef struct {
	char *path;
	char *name;
	char *label;
	int depth;
	int lock;
	int flag;
	NODE *deplist;
} DESC;

#define	DESC_BSIZE	100

static DESC *desc_tab = NULL;
static size_t desc_dim = 0;
static size_t desc_size = 0;

static DESC *desc_next (void)
{
	if	(desc_dim >= desc_size)
	{
		desc_size += DESC_BSIZE;
		desc_tab = xrealloc(desc_tab,
			desc_size * sizeof desc_tab[0]);
	}

	return desc_tab + desc_dim++;
}

static void desc_load (const char *name, int flag);

#define	MAXLINE	1023

static char *mstrdup (const char *str)
{
	if	(str)
	{
		int n = strlen(str) + 1;
		return memcpy(xmalloc(n), str, n);
	}

	return NULL;
}

static char *mstrndup (const char *str, size_t size)
{
	char *tg = xmalloc(size + 1);

	if	(str)
	{
		strncpy(tg, str, size);
		tg[size] = 0;
	}
	else	tg[0] = 0;

	return tg;
}

static char *catpath (const char *dir, const char *name)
{
	int n1 = strlen(dir);
	int n2 = strlen(name);
	char *p = xmalloc(n1 + n2 + 2);
	memcpy(p, dir, n1);

	if	(n1 && p[n1 - 1] != '/')
		p[n1++] = '/';

	memcpy(p + n1, name, n2 + 1);
	return p;
}

static char *get_line (FILE *file)
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
		x->name = mstrdup(name);
		x->next = desc->deplist;
		desc->deplist = x;
	}
}

static void desc_file (const char *name)
{
	FILE *file;
	DESC *desc;
	char *p;
	char *key;
	int i, i0, i1;

	file = fopen(name, "r");

	if	(!file)
		error(NOREAD, name);

	for (i = i0 = i1 = 0; name[i] != 0; i++)
	{
		if	(name[i] == '/' && i >= i1)
		{
			i0 = i1;
			i1 = i + 1;
		}
	}

	desc = desc_next();
	desc->path = mstrdup(name);

	if	(i1 > i0)
	{
		desc->name = mstrndup(name + i0, i1 - i0 - 1);
	}
	else	desc->name = mstrdup(name);

	desc->label = NULL;
	desc->deplist = NULL;
	desc->depth = 0;
	desc->lock = 0;
	desc->flag = 0;

	while ((p = get_line(file)))
	{
		if	((key = getkey(&p)) == NULL)
		{
			continue;
		}
		else if	(iskey("Name", key))
		{
			xfree(desc->name);
			desc->name = mstrdup(p);
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
			desc->label = mstrdup(p);
		}
	}

	fclose(file);
}

static void desc_dir (const char *path, int flag)
{
	struct dirent *entry;
	DIR *dir;

	if	(!flag)
	{
		char *p = catpath(path, "DESC");

		if	(access(p, R_OK) == 0)
		{
			desc_file(p);
			xfree(p);
			return;
		}

		xfree(p);
	}

	if	((dir = opendir(path)) == NULL)
	{
		perror(path);
		return;
	}

	while ((entry = readdir(dir)) != NULL)
	{
		if	(entry->d_name[0] != '.')
		{
			char *p = catpath(path, entry->d_name);
			desc_load(p, 0);
			xfree(p);
		}
	}

	closedir(dir);
}

static void desc_load (const char *path, int flag)
{
	struct stat buf;

	if	(stat(path,  &buf))	perror(path);
	else if	(S_ISDIR(buf.st_mode))	desc_dir(path, flag);
	else if	(flag)			desc_file(path);
}


static void desc_label (DESC *desc, size_t n)
{
	printf(LBLFMT, desc[n].name, desc[n].label ? desc[n].label : "");
}

static void desc_show (DESC *desc, size_t n)
{
	NODE *p;
	int i;

	printf("%s", desc[n].path);
	printf(" %s", desc[n].name);

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
	int n;
	char *p;
	void (*ofunc) (DESC *desc, size_t n);

	callname = argv[0];
	ofunc = desc_label;

	for (n = 1; n < argc; n++)
	{
		if	(argv[n][0] != '-')
		{
			break;
		}
		else if	(argv[n][1] == 'd')
		{
			ofunc = desc_show;
		}
		else	usage(argv[n]);
	}

	if	(argc == n)
		usage(NULL);

	if	((p = getenv("LANG")))
		Lang = mstrdup(p);

	argc -= n;
	argv += n;

	for (n = 0; n < argc; n++)
		desc_load(argv[n], 1);
	
	if	(desc_dim > 1)
	{
		for (n = 0; n < desc_dim; n++)
			desc_depth(desc_tab + n, desc_tab, desc_dim);

		qsort(desc_tab, desc_dim, sizeof desc_tab[0], cmp);
	}

	for (n = 0; n < desc_dim; n++)
		ofunc(desc_tab, n);

	return 0;
}
