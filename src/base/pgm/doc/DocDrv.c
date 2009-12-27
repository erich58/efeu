/*
Dokumenttreiberstruktur

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

#include <DocDrv.h>
#include <EFEU/preproc.h>
#include <EFEU/cmdeval.h>

#define	BLKS	32		/* Blockgröße der Variablentabelle */


/*	Initialisierung und Freigabe
*/

void *DocDrv_alloc (const char *name, size_t size)
{
	DocDrv *drv = memalloc(size);
	vb_init(&drv->varbuf, BLKS, sizeof(EfiVar));
	drv->vartab = VarTab(name, BLKS);
	DocDrv_var(drv, &Type_io, "plain", &drv->out);
	drv->name = name;
	return drv;
}

void DocDrv_free (void *ptr)
{
	DocDrv *drv = ptr;

	DelVarTab(drv->vartab);
	vb_free(&drv->varbuf);
	DocSym_free(drv->symtab);
	memfree(ptr);
}

/*	Hilfsfunktionen
*/

void DocDrv_var (void *ptr, EfiType *type, const char *name, void *data)
{
	DocDrv *drv = ptr;
	EfiVar *var = vb_next(&drv->varbuf);
	memset(var, 0, sizeof(EfiVar));
	var->name = (char *) name;
	var->type = type;
	var->data = data;
	AddVar(drv->vartab, var, 1);
}


void DocDrv_eval (void *ptr, const char *name)
{
	IO *in, *out;
	DocDrv *drv;
	
	drv = ptr;
	drv->submode++;
	in = io_findopen(CFGPATH, name, CFGEXT, "rd");
	in = io_cmdpreproc(in);
	out = DocDrv_io(drv);
	PushVarTab(RefVarTab(drv->vartab), NULL);
	CmdEval(in, out);
	PopVarTab();
	io_close(out);
	io_close(in);
	drv->submode--;
}

extern int DocDrv_plain (void *ptr, int c)
{
	DocDrv *drv = ptr;
	io_putc(c, drv->out);
	return c;
}

/*	Ausgabestruktur
*/

static int drv_put (int c, void *ptr)
{
	DocDrv *drv = ptr;

	if	(drv->skip)	return c;

	drv->last = drv->put ? drv->put(drv, c) : c;
	return c;
}

static int drv_sym (DocDrv *drv, const char *name)
{
	if	(!drv->sym)
	{
		char *sym = DocSym_get(drv->symtab, name);

		if	(sym)		io_puts(sym, drv->out);
		else			return EOF;
	}
	else	drv->sym(drv, name);

	drv->last = 0;
	return 0;
}

static int drv_ctrl (void *ptr, int req, va_list list)
{
	DocDrv *drv = ptr;
	int stat;

	switch (req)
	{
	case IO_CLOSE:
		if	(drv->submode)
			return 0;

		if	(drv->hflag && drv->hdr)
			drv->hdr(drv, 0);

		stat = io_close(drv->out);
		DocDrv_free(drv);
		return stat;
	case IO_IDENT:
		*va_arg(list, char **) = io_xident(drv->out,
			"%s(%*)", drv->name);
		break;
	case DOC_HEAD:
		if	(drv->hflag)	break;
		if	(drv->hdr)	drv->hdr(drv, 1);
		drv->hflag = 1;
		break;
	case DOC_REM:
		if	(drv->rem)	drv->rem(drv, va_arg(list, char *));
		break;
	case DOC_SYM:
		return drv->skip ? 0 : drv_sym(drv, va_arg(list, char *));
	case DOC_CMD:
		return drv->cmd ? drv->cmd(drv, list) : EOF;
	case DOC_BEG:
		return drv->env ? drv->env(drv, 1, list) : EOF;
	case DOC_END:
		return drv->env ? drv->env(drv, 0, list) : EOF;
	default:
		return EOF;
	}

	return 0;
}


IO *DocDrv_io (void *drv)
{
	IO *io = io_alloc();
	io->put = drv_put;
	io->ctrl = drv_ctrl;
	io->data = drv;
	return io;
}
