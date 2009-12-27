/*
:*:	cpp post filter for mkmf
:de:	Filterprogramm für mkmf

$Copyright (C) 1999 Erich Frühstück
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
#include <ctype.h>
#include <unistd.h>

/*
$pconfig
c|
	:*:converts LF to CR/LF
	:de:Konvertiert LF nach CR/LF
*/

static void usage (const char *name, const char *arg)
{
	execlp("efeuman", "efeuman", "--", __FILE__, arg, NULL);
	fprintf(stderr, "usage: %s [-c]", name);
}

/*
$Description
:*:The command |$!| is used by |ccmkmf| to filter the output of the
C-preprocessor. The following options are accepted:
:de:Das Kommando |$!| wird von |ccmkmf| zum Filtern der Ausgabe des
C-Präprozessors verwendet. Die folgenden Optionen werden vom Kommando
akzeptiert.

@arglist

$Notes
:*:
The check of options is done in a very simple form without
error diagnostics, because
the command is normally used only by |ccmkmf|.
:de:
Da das Kommando nicht direkt aufgerufen wird, wurde auf eine
genaue Überprüfung der Argumentliste verzichtet. Nur das erste
Argument wird analysiert.

$SeeAlso
\mref{efeucc(1)}, \mref{ccmkmf(1)}, \mref{pp2dep(1)}, \mref{cc(1)}.
*/

static int prefilter (void);
static int expand (void);
static int clearspace (void);
static void postfilter (int c);
static int copystr (void);

int cr_flag = 0;
int str_flag = 0;

typedef struct tmpstack_s tmpstack_t;

struct tmpstack_s {
	FILE *file;
	tmpstack_t *next;
};

static tmpstack_t *copy = NULL;
static tmpstack_t *save = NULL;


/*	Hauptprogramm
*/

int main (int narg, char **arg)
{
	int c;

	cr_flag = 0;

	if	(narg > 1)
	{
		if	(strcmp("-?", arg[1]) == 0)
			usage(arg[0], NULL);
		else if	(strncmp("--help", arg[1], 6) == 0)
			usage(arg[0], arg[1]);
		else if	(strcmp("-c", arg[1]) == 0)
			cr_flag = 1;
	}

	do	c = clearspace();
	while	(c == '\n');

	do
	{
		if	(copy)
		{
			putc(c, copy->file);
		}
		else	postfilter(c);

		c = clearspace();
	}
	while	(c != EOF);
	
	while (save)
	{
		tmpstack_t *x = save;
		save = x->next;

		while ((c = getc(x->file)) != EOF)
			postfilter(c);

		fclose(x->file);
		free(x);
	}

	return 0;
}


/*	Temporärer Buffer
*/

static void open_tmp (void)
{
	tmpstack_t *x = malloc(sizeof(tmpstack_t));
	x->file = tmpfile();
	x->next = copy;
	copy = x;
}

static void close_tmp (void)
{
	if	(copy)
	{
		tmpstack_t *x = copy;
		rewind(x->file);
		copy = x->next;
		x->next = save;
		save = x;
	}
}


/*	String kopieren
*/

static int copystr (void)
{
	int c;

	while ((c = getchar()) != EOF)
	{
		if	(c == '\\')
		{
			c = getchar();

			switch (c)
			{
			case EOF:	return 1;
			case '\n':	continue;
			case 'a':	c = '\a'; break;
			case 'b':	c = '\b'; break;
			case 'f':	c = '\f'; break;
			case 'n':	c = '\n'; break;
			case 'r':	c = '\r'; break;
			case 't':	c = '\t'; break;
			case 'v':	c = '\v'; break;
			case 'x':	scanf("%2x", &c); break;
			case '0':	scanf("%3o", &c); break;
			default:	break;
			}
		}
		else if	(c == '"')
		{
			return 0;
		}

		putchar(c);
	}

	return 1;
}

/*	Basisfilter: Entfernt Kommentarzeilen
*/

static int prefilter (void)
{
	static int line_start = 1;
	static int skip = 0;
	int c;

	while ((c = getchar()) != EOF)
	{
		if	(skip)
		{
			if	(c == '\n')
			{
				skip = 0;
				break;
			}
		}
		else if	(line_start && c == ' ')
		{
			;
		}
		else if	(line_start && c == '#')
		{
			skip = 1;
		}
		else	break;
	}

	line_start = (c == '\n');
	return c;
}


/*	Filter zum Expandieren von Sonderzeichen
*/

int expand (void)
{
	static int save = 0;
	int c;

	if	(save != 0)
	{
		c = save;
		save = 0;
		return c;
	}

	while ((c = prefilter()) == '@')
	{
		c = prefilter();

		switch (c)
		{
		case '.':	continue;
		case '!':	c = '#'; break;
		case ':':	c = ','; break;
		case '+':	c = '\t'; break;
		case ';':	c = '\n'; break;
		case '/':	c = '\\'; break;
		case '@':	c = '@'; break;
		case '|':	c = '\''; break;
		case '"':	c = 0; str_flag = 1; break;
#if	0
		case '*':	c = 0; break;
		case '{':	save = '*'; c = '/'; break;
		case '}':	save = '/'; c = '*'; break;
#else
		case '{':	open_tmp(); c = 0; break;
		case '}':	close_tmp(); c = 0; break;
#endif
		default:	save = c; c = '@'; break;
		}

		break;
	}

	return c;
}


int clearspace (void)
{
	static int ign_space = 1;
	static int save = 0;
	int space_flag;
	int c;

	if	(save != 0)
	{
		c = save;
		save = 0;
		return c;
	}

	if	(str_flag)
	{
		str_flag = 0;

		if	(copystr())
			return EOF;
	}
	
	space_flag = 0;

	while ((c = expand()) == ' ')
		space_flag = !ign_space;

	if	(c == '\t' || c == '\n')
	{
		space_flag = 0;
		ign_space =  1;
	}
	else	ign_space = 0;

	if	(space_flag)
	{
		save = c;
		c = ' ';
	}

	return c;
}


/*	Ausgabefilter: Entfernt Tabulatoren am Ende einer Zeile und
	reduziert aufeinanderfolgende Leerzeilen auf maximal 2.
*/

static void postfilter (int c)
{
	static int nl_count = 0;
	static int tab_count = 0;

	switch (c)
	{
	case '\t':

		tab_count++;
		return;

	case '\n':

		tab_count = 0;

		if	(nl_count > 1)
			return;

		nl_count++;

		if	(cr_flag)
			putchar('\r');

		break;

	default:

		nl_count = 0;

		for (; tab_count > 0; tab_count--)
			putchar('\t');

		break;
	}

	if	(c)
		putchar(c);
}

