/*
Ausgabefilter für Terminal

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

#include <term.h>
#include <efeudoc.h>
#include <ctype.h>

#define	TPARM	"tparm"	/* Terminalkonfiguration */
#define	NAME	"term"	/* Name der Variablentabelle */
#define	HDRCFG	"term"	/* Kopfkonfiguration */
#define	SYMTAB	"term"	/* Sonderzeichentabelle */

TermPar term_par;

static EfiVarDef term_var[] = {
	{ "wpmargin", &Type_int, &term_par.wpmargin },
	{ "rm", &Type_str, &term_par.rm },
	{ "it", &Type_str, &term_par.it },
	{ "bf", &Type_str, &term_par.bf },
	{ "tt", &Type_str, &term_par.tt },
	{ "Name", &Type_str, &term_par.Name },
	{ "FigName", &Type_str, &term_par.FigName },
	{ "TabName", &Type_str, &term_par.TabName },
};

void TermPar_init(void)
{
	static int need_init = 1;

	if	(need_init)
	{
		IO *in;

		in = io_findopen(CFGPath, TPARM, CFGEXT, "r");
		in = io_lnum(in);
		PushVarTab(NULL, NULL);
		AddVarDef(NULL, term_var, tabsize(term_var));
		io_eval(in, NULL);
		PopVarTab();
		io_close(in);
		need_init = 0;
	}
}

static void term_hdr (void *drv, int mode)
{
	if	(mode)	DocDrv_eval(drv, HDRCFG);
}

static void term_sym (void *drv, const char *name)
{
	Term *trm = drv;

	if	(mstrcmp("nbsp", name) == 0)
	{
		term_hmode(trm);
		io_putc(' ', trm->out);
		trm->col++;
	}
	else if	(mstrcmp("shy", name) == 0)
	{
		if	(trm->col >= term_par.wpmargin)
		{
			io_putc('-', trm->out);
			term_newline(trm, 0);
		}
	}
	else
	{
		char *sym = DocSym_get(trm->symtab, name);

		term_hmode(trm);

		if	(sym)
		{
			io_puts(sym, trm->out);
			trm->col++;
		}
		else	trm->col += io_xprintf(trm->out, "[%s]", name);
	}
}

IO *DocOut_term (IO *io)
{
	Term *trm = DocDrv_alloc(NAME, sizeof(Term));
	trm->out = io;
	trm->var.margin = 2 * TERM_INDENT;
	trm->symtab = DocSym_load(SYMTAB);
	trm->sym = term_sym;
	trm->put = term_putc;
	trm->hdr = term_hdr;
	trm->cmd = term_cmd;
	trm->env = term_env;
	trm->tab = NULL;
	trm->save_out = NULL;
	AddVarDef(trm->vartab, term_var, tabsize(term_var));
	DocDrv_var(trm, &Type_int, "margin", &trm->var.margin);
	TermPar_init();
	return DocDrv_io(trm);
}
