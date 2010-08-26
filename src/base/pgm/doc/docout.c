/*
Dokumentausgabefilter

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

#include <DocOut.h>
#include <EFEU/mstring.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>
#include <EFEU/Info.h>
#include <EFEU/MakeDepend.h>
#include <EFEU/Debug.h>
#include <EFEU/ftools.h>
#include <EFEU/preproc.h>

extern char *CFGPath;

static VECBUF(type_tab, 32, sizeof(DocType *));

void AddDocType (DocType *type)
{
	vb_append(&type_tab, &type, 1);
}

static DocType *get_type (const char *name)
{
	size_t n;
	DocType **p;

	for (n = type_tab.used, p = type_tab.data; n--; p++)
		if (mstrcmp(name, (*p)->name) == 0) return *p;

	return NULL;
}

extern IO *html_open(const char *dir, const char *pfx);
static void print_type (IO *io, InfoNode *base);

/*	Postfilterausgabe
*/

static IO *post_open (const char *name, const char *par)
{
	if	(MakeDepend)
	{
		AddTarget(name);
		return NULL;
	}

	if	(name)
	{
		char *p = mstrpaste(*name == '|' ? NULL : " > ", par, name);
		IO *io = io_popen(p, "w");
		memfree(p);
		return io;
	}

	return io_popen(par, "w");
}

/*	Tabelle mit Ausgabefunktionen
*/

static DocType TypeTab[] = {
	{ "test", ":*:test output"
		":de:Testausgabe",
		DocOut_test, NULL, NULL },
	{ "term", ":*:terminal output"
		":de:Terminalausgabe",
		DocOut_term, NULL, NULL },
	{ "LaTeX", ":*:LaTeX source"
		":de:LaTeX-Dokument",
		DocOut_latex, NULL, NULL },
	{ "tex2ps", ":*:PostScript over LaTeX"
		":de:PostScript-Dokument über LaTeX",
		DocOut_latex, post_open, "tex2ps -n3 -" },
	{ "tex2pdf", ":*:PDF over LaTeX"
		":de:PDF-Dokument über LaTeX",
		DocOut_latex, post_open, "tex2pdf -n3 -" },
	{ "man", ":*:man source"
		":de:Handbuchsource",
		DocOut_mroff, NULL },
	{ "nroff", ":*:nroff formatted manpage"
		":de:Nroff-formatiertes Handbuch",
		DocOut_mroff, post_open, "groff -Tlatin1 -t -man" },
	{ "html", ":*:HTML output"
		":de:HTML Ausgabe",
		DocOut_html, html_open, NULL },
};


static void SetupDocType (void)
{
	static int setup_done = 0;

	if	(!setup_done)
	{
		int i;

		for (i = 0; i < tabsize(TypeTab); i++)
			AddDocType(TypeTab + i);

		setup_done = 1;
	}
}

DocType *GetDocType (const char *name)
{
	DocType *type;

	if	(!name)	return NULL;

	SetupDocType();

	if	(*name == '?')
	{
		print_type(iostd, NULL);
		exit(EXIT_SUCCESS);
	}

	if	(!(type = get_type(name)))
	{
		char *fname;

		if	((fname = fsearch(CFGPath, NULL, name, "so")))
		{
			char *setup = mstrpaste("_", name, "setup");
			loadlib(fname, setup);
			memfree(setup);
			memfree(fname);
			type = get_type(name);
		}
	}

	if	(!type)
	{
		log_psubarg(ErrLog, "[Doc:1]", NULL, "s", name);
		print_type(ioerr, NULL);
		exit(EXIT_FAILURE);
	}


	return type;
}

/*	Ausgabefilter auflisten
*/

static void print_type (IO *io, InfoNode *base)
{
	size_t n;
	DocType **p;

	for (n = type_tab.used, p = type_tab.data; n--; p++)
	{
		io_puts((*p)->name, io);
		io_putc('\t', io);
		io_langputs((*p)->desc, io);
		io_putc('\n', io);
	}
}

void DocOutInfo (const char *name, const char *desc)
{
	AddInfo(NULL, name, desc, print_type, NULL);
}


/*	Dokumentausgabe öffnen
*/

IO *DocFilter (DocType *type, IO *io)
{
	return (type && type->filter) ? type->filter(io) : io;
}

IO *DocOut (DocType *type, const char *name)
{
	IO *io;

	if	(type == NULL)
		return io_fileopen(name, "wzd");

	if	(type->docopen)
		io = type->docopen(name, type->par);
	else	io = io_fileopen(name, "wzd");

	if	(type->filter)
		io = type->filter(io);

	return io;
}
