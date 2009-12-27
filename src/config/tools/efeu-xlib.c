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

/*
$pconfig
m::name |
	:*:define macro name <name> with found flags.
	:de:Makro <name> mit gefundenen Flags initialisieren.
d::level |
	:*:max level of recursvive search.
	:de:Maximale Ebene bei rekursiver Suche.
I::path |
	:*:path to search header files (default: |/usr/include|).
	:de:Suchpfad für Kopfdateien, Vorgabe: |/usr/include|.
L::path |
	:*:path to search libraries (default: |/lib:/usr/lib|).
	:de:Suchpfad für Bibliotheken, Vorgabe: |/lib:/usr/lib|.
a |
	:*:try next search only if last search was successfull.
	:de:Nächste Suche nur durchführen, wenn letzte Suche erfolgreich war.
o |
	:*:try next search only if last search failed.
	:de:Nächste Suche nur durchführen, wenn letzte Suche fehlschlug.
l:lib |
	:*:search for libraray corresponding to the regular
	expression |^lib(|<lib>|)\.(a||so)$$|.
	:de:Suche nach der Bibliothek, die dem regulären
	Ausdruck |^lib(|<lib>|)\.(a||so)$$| entspricht.
::name |
	:*:search for header <name>, where <name> may contain
	directory parts as in |X11/X.h|.
	:de:Suche nach der Kopfdatei <name>, wobei <name>
	Pfadteile enthalten kann (z.B: |X11/X.h|)
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
:*:The command |$1| is used by configuration tools to determine
install places of external libraries and header files. A |//| in
a path means to search subdirectories recursively.
If no header file or library is given, the command lists all directories
where files are searched.
:de:Das Kommando |$1| wird von Konfigurationswerkzeugen zur Bestimmung
des Installationsortes von externen Programmbibliotheken verwendet.
Ein |//| in einem Pfad bewirkt eine rekursive Suche von Teilverzeichnissen.
Falls keine Kopfdatei oder Bibliothek angegeben wurde, werden alle
Verzeichnisse aufgelistet.

@arglist -i

:*:No space beetwen option key and option argument is allowed.
If |-L| or |-I| is used without any argument, the corresponding path
is set to null.
:de:Zwischen Optionskennung und Optionsargument darf kein Leerzeichen
stehen. Falls |-L| oder |-I| ohne Argument verwendet wird, wird der
zugehörige Suchpfad auf |NULL| gesetzt.

$Example
:*:The command
:de:Der Befehl

|$! -L/usr/local//lib:/opt//lib| -lreadline

:*:searches for the library |readline| in the default places |/lib|,
|/usr/lib| and all subdirectories with name |lib| of |/usr/local| and
|/opt|.
:de:sucht nach der Bibliothek |readline| in den Standardorten |/lib|,
|/usr/lib| und allen Unterverzeichnissen von |/usr/local| und |/opt|
mit Namen |lib|.

|$! -lncurses -o -lcurses|

:*:Checks if library |ncurses| is available or if |curses| should be used
instead
:de:Prüft, ob |ncurses| verfügbar ist oder ob |curses| stellvertretend
verwendet werden soll.

$SeeAlso
efeuconfig(1).
*/

/*	string tools
*/

static void *xrealloc (void *buf, size_t n)
{
	void *p = realloc(buf, n);

	if	(p)	return p;

	fprintf(stderr, "%s: memmory allocation failed.\n", callname);
	exit(EXIT_FAILURE);
}

static void xfree (void *p)
{
	if	(p)	free(p);
}

static char *mstrncpy (const char *str, size_t n)
{
	char *tg = xrealloc(NULL, n + 1);
	strncpy(tg, str, n);
	tg[n] = 0;
	return tg;
}

static char *catpath (const char *base, const char *sub)
{
	int n1, n2;
	char *tg;

	n1 = base ? strlen(base) : 0;

	while (n1 && base[n1 - 1] == '/')
		n1--;

	while	(*sub == '/')	sub++;

	n2 = strlen(sub);

	tg = xrealloc(NULL, n1 + n2 + 2);
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
	base = xrealloc(base, s1 + s2 + 2);

	if	(s1)	base[s1++] = ':';

	strcpy(base + s1, sub);
	return base;
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
	test_buf = xrealloc(test_buf, strlen(path) + strlen(name) + 2);
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
			sub = mstrncpy(path, p - path);
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
		char *sub = mstrncpy(path, p - path);
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
