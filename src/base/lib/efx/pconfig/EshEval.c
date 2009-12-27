/*
Interpreter initialisieren

$Copyright (C) 1995 Erich Frühstück
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

#include <EFEU/parsedef.h>
#include <EFEU/pconfig.h>
#include <EFEU/CmdPar.h>
#include <EFEU/Resource.h>
#include <EFEU/Debug.h>
#include <EFEU/preproc.h>

#define	ESH_PFX	"scripts"

static int *pgm_argc = NULL;
static char **pgm_argv = NULL;

static EfiObj *pf_pconfig (IO *io, void *data);

static EfiObj *pf_call (IO *io, void *data)
{
	return Obj_call(data, NULL, NULL);
}

static EfiObj *expr_argc (void *par, const EfiObjList *list)
{
	return int2Obj(*pgm_argc);
}

static EfiObj *expr_argv (void *par, const EfiObjList *list)
{
	return EfiVecObj(&Type_str, pgm_argv, *pgm_argc);
}

static EfiParseDef esh_pdef[] = {
	{ "argc", pf_call, expr_argc },
	{ "argv", pf_call, expr_argv },
	{ "pconfig", pf_pconfig, NULL },
};


static void f_shift (EfiFunc *func, void *rval, void **arg);

int EshEval (int *narg, char **arg)
{
	int i;
	CmdPar *par;
	char *name;
	char *p;
	IO *in;

	SetProgName(arg[0]);
	SetupStd();

	par = CmdPar_alloc(ProgName);
	p = mstrcpy(CmdPar_getval(NULL, "Version", NULL));
	CmdPar_setval(par, "Version", p);
	CmdPar_load(par, "efm", 0);
	CmdPar_load(par, "eshconf", 0);
	CmdPar_load(par, par->name, 1);

	if	(CmdPar_eval(par, narg, arg, 1) <= 0)
		return EXIT_FAILURE;

	DebugMode(CmdPar_getval(par, "Debug", NULL));
	SetFunc(0, &Type_void, "shift (int n = 1)", f_shift);
	AddParseDef(esh_pdef, tabsize(esh_pdef));

	pgm_argc = narg;
	pgm_argv = arg;

	if	((p = CmdPar_getval(par, "CmdString", NULL)))
	{
		in = io_mstr(mstrcpy(p));
	}
	else if	(*narg > 1 && pgm_argv[1][0] != '-')
	{
		(*pgm_argc)--;

		for (i = 0; i < *pgm_argc; i++)
			pgm_argv[i] = pgm_argv[i + 1];

		memfree(ProgIdent);
		ProgIdent = mstrpaste(" ", ProgName, pgm_argv[0]);

		memfree(ProgName);

		if	(pgm_argv[0] && pgm_argv[0][0] == '/')
		{
			ProgName = mstrcpy(strrchr(pgm_argv[0], '/') + 1);
		}
		else	ProgName = mstrcpy(pgm_argv[0]);

		name = fsearch(IncPath, ESH_PFX, arg[0], 
			CmdPar_getval(par, "CmdType", NULL));
		in = io_fileopen(name ? name : arg[0], "r");
		memfree(name);

		par = CmdPar_ptr(NULL);
		memfree(par->name);
		par->name = mstrcpy(ProgName);
	}
	else
	{
		if	(*narg > 1)
			skiparg(narg, arg, 1);

		in = io_interact(NULL, CmdPar_getval(par, "HistoryName", NULL));
	}

	in = io_cmdpreproc(in);

	if	(atoi(CmdPar_getval(par, "PPOnly", "0")))
	{
		int c;

		while ((c = io_getc(in)) != EOF)
			io_putc(c, iomsg);
	}
	else
	{
		EshIdent(in);
		CmdEval(in, iomsg);
	}

	io_close(in);
	return EXIT_SUCCESS;
}


/*	Makrodefinitionen
*/

static EfiObj *pf_pconfig (IO *io, void *data)
{
	if	(io_eat(io, "%s") == '!')
	{
		int c;

		do	c = io_getc(io);
		while	(c != EOF && c != '\n');

		CmdPar_read(NULL, io, '!', 0);
	}

	CmdPar_info(NULL, NULL);

	if	(CmdPar_eval(NULL, pgm_argc, pgm_argv, 0) <= 0)
		exit(EXIT_FAILURE);

	DebugMode(GetResource("Debug", NULL));
	return NULL;
}


/*	Funktionen
*/

static void f_shift (EfiFunc *func, void *rval, void **arg)
{
	int i, n;

	n = Val_int(arg[0]);

	if	(n >= *pgm_argc)
		n = *pgm_argc - 1;

	if	(n > 0)
	{
		*pgm_argc -= n;

		for (i = 1; i < *pgm_argc; i++)
			pgm_argv[i] = pgm_argv[i + n];
	}
}
