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
#include <EFEU/nkt.h>

int pctrl_fsize = 10;
char *pctrl_pgfmt = "a4";

static int pf_put (int c, IO *io);
static int pf_ctrl (IO *io, int req, va_list list);

static NameKeyTab pctrl_deftab = NKT_DATA("pctrl", 32, NULL);

int pctrl_add (PCTRLDef *def)
{
	return def ? nkt_insert(&pctrl_deftab, def->name, def) : 0;
}

static void pctrl_init()
{
	if	(pctrl_deftab.tab.used == 0)
	{
		pctrl_add(&pctrl_std);
		pctrl_add(&pctrl_test);
		pctrl_add(&pctrl_ascii);
		pctrl_add(&pctrl_struct);
		pctrl_add(&pctrl_csv);
		pctrl_add(&pctrl_data);
		pctrl_add(&pctrl_sc);
		pctrl_add(&pctrl_tex);
		pctrl_add(&pctrl_tab);
	}
}

static PCTRLDef *get_pctrl (const char *name)
{
	pctrl_init();
	return nkt_fetch(&pctrl_deftab, name, &pctrl_std);
}

static int pctrl_visit(const char *name, void *data, void *par)
{
	PCTRLDef *def = data;
	io_printf(par, "%s\t%s\n", def->name, def->desc);
	return 0;
}

void pctrl_list(IO *out)
{
	pctrl_init();
	nkt_walk(&pctrl_deftab, pctrl_visit, out);
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

static int pf_put (int c, IO *io)
{
	PCTRL *pf = io->data;
	return pf->put ? pf->put(c, pf) : 0;
}


/*	Kontrollfunktion
*/

static int pf_ctrl (IO *io, int req, va_list list)
{
	PCTRL *pf = io->data;

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

	return EOF;
}
