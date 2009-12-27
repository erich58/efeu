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


extern io_t *html_open(const char *dir, const char *pfx);

/*	Postfilterausgabe
*/

static io_t *post_open (const char *name, const char *par)
{
	if	(MakeDepend)
	{
		AddTarget(name);
		return NULL;
	}

	if	(name)
	{
		char *p = mstrpaste(*name == '|' ? NULL : " > ", par, name);
		io_t *io = io_popen(p, "w");
		memfree(p);
		return io;
	}

	return io_popen(par, "w");
}

/*	Tabelle mit Ausgabefunktionen
*/

static DocType_t TypeTab[] = {
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


DocType_t *GetDocType (const char *name)
{
	int i;

	for (i = 0; i < tabsize(TypeTab); i++)
		if	(mstrcmp(name, TypeTab[i].name) == 0)
			return TypeTab + i;

	return NULL;
}

/*	Ausgabefilter auflisten
*/

static void print_type (io_t *io, InfoNode_t *base)
{
	int i;

	for (i = 0; i < tabsize(TypeTab); i++)
	{
		io_puts(TypeTab[i].name, io);
		io_putc('\t', io);
		io_langputs(TypeTab[i].desc, io);
		io_putc('\n', io);
	}
}

void DocOutInfo (const char *name, const char *desc)
{
	AddInfo(NULL, name, desc, print_type, NULL);
}


/*	Dokumentausgabe öffnen
*/

io_t *DocFilter (DocType_t *type, io_t *io)
{
	return (type && type->filter) ? type->filter(io) : io;
}

io_t *DocOut (DocType_t *type, const char *name)
{
	io_t *io;

	if	(type == NULL)
		return io_fileopen(name, "wzd");

	if	(type->docopen)
		io = type->docopen(name, type->par);
	else	io = io_fileopen(name, "wzd");

	if	(type->filter)
		io = type->filter(io);

	return io;
}
