/*
:*:mdprint output control 
:de:Ausgabekontrolle für mdprint

$Copyright (C) 1994, 2002 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/pctrl.h>
#include <EFEU/ioctrl.h>

int pctrl_fsize = 10;
char *pctrl_pgfmt = "a4";

static int pf_put (int c, void *pf);
static int pf_ctrl (void *data, int req, va_list list);

typedef struct {
	char *name;	/* Name */
	char *desc;	/* Beschreibung */
	const char *nc;	/* Neue Spalte */
	const char *nl;	/* Neue Zeile */
	const char *nf;	/* Neue Seite */
	int (*ctrl) (PCTRL *io, int cmd, va_list list);
} PCTRLDef;

static PCTRLDef pctrl[] = {
	{ NULL, "Standardausgabefilter", " ", "\n", "\f", std_ctrl },
	{ "test", "Testausgabefilter", " ", "<NL>\n", "<FF>\f", tst_ctrl },
	{ "ascii", "Ascii Ausgabe", "\t", "\n", "\f", std_ctrl },
	{ "struct", "Formatierung als Liste", ", ", " },\n", NULL, struct_ctrl },
	{ "csv", "Formatierung als csv - Datei", ";", "\r\n", NULL, csv_ctrl },
	{ "data", "Nur Datenwerte ausgeben", "\t", "\n", NULL, data_ctrl },
	{ "sc", "Aufbereitung für SC", "\n", "\n", NULL, sc_ctrl },
	{ "tex", "LaTeX-Dokument", "&", "~\\\\\n", "\\newpage\n", tex_ctrl },
	{ "tab", "LaTeX-Tabelle", "&", "~\\\\\n", "\\newpage\n", tab_ctrl },
};

static PCTRLDef *get_pctrl (const char *name)
{
	int i;

	for (i = 0; i < tabsize(pctrl); i++)
		if (mstrcmp(pctrl[i].name, name) == 0)
			return pctrl + i;

	return pctrl;
}

/*	Druckkontrolle generieren
*/

IO *io_pctrl(IO *io, const char *name)
{
	if	(io)
	{
		PCTRLDef *pc = get_pctrl(name);
		PCTRL *pf = memalloc(sizeof(PCTRL));
		pf->io = io;
		pf->line = 0;
		pf->col = 0;
		pf->page = 0;
		pf->delim = PrintListDelim;
		pf->nl = pc->nl;
		pf->nf = pc->nf;
		pf->put = NULL;
		pf->ctrl = pc->ctrl;

		io = io_alloc();
		io->put = pf_put;
		io->ctrl = pf_ctrl;
		io->data = pf;
		io_ctrl(io, PCTRL_BEGIN);
	}

	return io;
}


/*	Zeichen ausgeben
*/

static int pf_put (int c, void *data)
{
	PCTRL *pf = data;
	return pf->put ? pf->put(c, pf) : 0;
}


/*	Kontrollfunktion
*/

static int pf_ctrl (void *data, int req, va_list list)
{
	PCTRL *pf = data;

	if	(req == IO_CLOSE)
	{
		int stat;

		if	(pf->ctrl)
			pf->ctrl(pf, PCTRL_END, list);

		stat = io_close(pf->io);
		memfree(pf);
		return stat;
	}

	if	((req & ~0xff) != PCTRL_MASK)
		return io_vctrl(pf->io, req, list);

	if	(pf->put)
	{
		pf->put(EOF, pf);
		pf->put = NULL;
	}

	switch (req)
	{
	case PCTRL_YPOS:	return pf->line;
	case PCTRL_XPOS:	return pf->col;

	case PCTRL_BEGIN:
	
		pf->col = 0;
		pf->line = 1;
		pf->page = 1;
		break;

	case PCTRL_LINE:
	
		io_puts(pf->nl, pf->io);
		pf->col = 0;
		pf->line++;
		break;

	case PCTRL_PAGE:
	
		io_puts(pf->nf, pf->io);
		pf->line = 1;
		pf->page++;
		break;

	case PCTRL_LEFT:
	case PCTRL_CENTER:
	case PCTRL_RIGHT:
	case PCTRL_EMPTY:
	case PCTRL_VALUE:
	
		if	(pf->col++)	io_puts(pf->delim, pf->io);

		break;
	}

	if	(pf->ctrl)
		return pf->ctrl(pf, req, list);
	else	return EOF;
}
