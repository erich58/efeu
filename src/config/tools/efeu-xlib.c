/*
:*:	determine c-flags for use of external libraries
:de: 	Bestimmung von C-Flags zur Einbindung externer Bibliotheken

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
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <regex.h>

#define	USAGE	"usage: %s [-m<mac>] [-d<level>] -p<path> [-a] [-o] -l<lib> [name ...\n"

static char *callname = NULL;

static void usage (const char *arg)
{
	execlp("efeuman", "efeuman", callname, arg, NULL);
	fprintf(stderr, USAGE, callname);
	exit(arg ? 0 : 1);
}

/*	memmory allocation
*/

static void *xmalloc (void *data, size_t size)
{
	char *p;

	if	(!(p = realloc(data, size)))
		fprintf(stderr, "realloc(%p, %lu) failed.\n",
			data, (unsigned long) size);

	return p;
}

static void xfree (void *data)
{
	if	(data)	free(data);
}


/*	string tools
*/

static char *catpath (const char *base, const char *sub)
{
	int n1, n2;
	char *tg;

	n1 = base ? strlen(base) : 0;

	while (n1 && base[n1 - 1] == '/')
		n1--;

	while	(*sub == '/')	sub++;

	n2 = strlen(sub);

	tg = xmalloc(NULL, n1 + n2 + 2);
	strncpy(tg, base, n1);
	tg[n1] = '/';
	strncpy(tg + n1 + 1, sub, n2);
	tg[n1 + n2 + 1] = 0;
	return tg;
}

static char *expand (char *base, const char *sub)
{
	int s1, s2;

	if	(sub == NULL || *sub == 0)
	{
		xfree(base);
		return NULL;
	}

	s1 = base ? strlen(base) : 0;
	s2 = strlen(sub);
	base = xmalloc(base, s1 + s2 + 2);

	if	(s1)	base[s1++] = ':';

	strcpy(base + s1, sub);
	return base;
}

static char *substr (const char *str, size_t size)
{
	char *tg = xmalloc(NULL, size + 1);
	strncpy(tg, str, size);
	tg[size] = 0;
	return tg;
}

/*	test functions
*/

static char *macname = NULL;
static int maxdepth = 16;
static char *name = NULL;
static char *test_buf = NULL;
static int nout = 0;

static int test_list (const char *path)
{
	printf("%s\n", path);
	return 0;
}

static int test_hdr (const char *path)
{
	test_buf = xmalloc(test_buf, strlen(path) + strlen(name) + 2);
	sprintf(test_buf, "%s/%s", path, name);

	if	(access(test_buf, R_OK) != 0)
		return 0;

	if	(nout)		putchar(' ');
	else if	(macname)	printf("%s= ", macname);

	if	(strcmp("/usr/include", path) == 0)	;
	else						printf("-I%s", path);

	nout++;
	return 1;
}

static regex_t regex;

static void init_lib (const char *name)
{
	char buf[256];
	int n;

	sprintf(buf, "^lib(%s)\\.(a|so)$", name);
	n = regcomp(&regex, buf, REG_EXTENDED);

	if	(n)
	{
		regerror(n, &regex, buf, sizeof(buf));
		fprintf(stderr, "%s: libname \"%s\": %s\n",
			callname, name, buf);
		exit(1);
	}
}

static void libdir (const char *path)
{
	if	(nout)		putchar(' ');
	else if	(macname)	printf("%s= ", macname);

	if	(strcmp("/lib", path) == 0)	return;
	if	(strcmp("/usr/lib", path) == 0)	return;

	printf("-L%s ", path);
}

static int test_lib (const char *path)
{
	regmatch_t pmatch[2];
	struct dirent *entry;
	DIR *dir;
	int stat;

	if	((dir = opendir(path)) == NULL)
		return 0;

	stat = 0;

	while ((entry = readdir(dir)) != NULL)
	{
		if	(regexec(&regex, entry->d_name, 2, pmatch, 0) == 0)
		{
			int i;

			libdir(path);
			printf("-l");

			for (i = pmatch[1].rm_so; i < pmatch[1].rm_eo; i++)
				putchar(entry->d_name[i]);

			stat = 1;
			nout++;
			break;
		}
	}

	closedir(dir);
	return stat;
}

static void exit_lib (void)
{
	regfree(&regex);
}


static int (*test) (const char *path) = NULL;

/*	search functions
*/

static int search (int depth, const char *path);
static int subsearch (int depth, const char *path, const char *sub);

static int subsearch (int depth, const char *path, const char *sub)
{
	DIR *dir;
	char *p;
	int flag;
	struct dirent *entry;
	struct stat statbuf;

	if	(*sub)
	{
		p = catpath(path, sub);
		flag = search(depth, p);
		xfree(p);
	}
	else	flag = test(path);

	if	(flag)	
		return 1;

	if	(depth >= maxdepth)
		return 0;

	if	((dir = opendir(path)) == NULL)
		return 0;

	flag = 0;
	depth++;

	while (!flag && (entry = readdir(dir)) != NULL)
	{
		if	(entry->d_name[0] == '.')	continue;

		p = catpath(path, entry->d_name);

		if	(stat(p, &statbuf) == 0 && S_ISDIR(statbuf.st_mode))
			flag = subsearch (depth, p, sub);

		xfree(p);
	}

	closedir(dir);
	return flag;
}

static int search (int depth, const char *path)
{
	const char *p;
	char *sub;
	int flag;

	sub = NULL;
	flag = 0;

	for (p = path; *p != 0; p++)
	{
		if	(p[0] == '/' && p[1] == '/')
		{
			sub = substr(path, p - path);
			p += 2;
			path = sub;
			break;
		}
	}

	flag = sub ? subsearch(depth, sub, p) : test(path);
	xfree(sub);
	return flag;
}

static int psearch (const char *path)
{
	const char *p;

	if	(!path)	return 0;

	while ((p = strchr(path, ':')) != NULL)
	{
		char *sub = substr(path, p - path);
		int flag = search(0, sub);
		path = p + 1;

		if	(flag)	return 1;
	}

	return search(0, path);
}

/*	main function
*/

int main (int argc, char **argv)
{
	char *ipath;
	char *lpath;
	int last;
	int i;

	callname = argv[0];

	if	(argc <=1 )
		usage("-?");
	
	ipath = expand(NULL, "/usr/include");
	lpath = expand(NULL, "/lib:/usr/lib");
	last = 0;

	for (i = 1; i < argc; i++)
	{
		if	(argv[i][0] != '-')
		{
			name = argv[i];
			test = test_hdr;
			last = psearch(ipath);
			continue;
		}

		switch (argv[i][1])
		{
		default:	usage(argv[i]); break;
		case 'a':	if (!last) i++; break;
		case 'o':	if (last) i++; break;
		case 'm':	macname = argv[i] + 2; break;
		case 'd':	maxdepth = atoi(argv[i] + 2); break;
		case 'I':	ipath = expand(ipath, argv[i] + 2); break;
		case 'L':	lpath = expand(lpath, argv[i] + 2); break;
		case 'l':
			init_lib(argv[i] + 2);
			test = test_lib;
			last = psearch(lpath);
			exit_lib();
			break;
		}
	}

	if	(nout)
	{
		putchar('\n');
		return 0;
	}

	if	(test == NULL)
	{
		test = test_list;
		psearch(ipath);
		psearch(lpath);
	}

	return 1;
}
