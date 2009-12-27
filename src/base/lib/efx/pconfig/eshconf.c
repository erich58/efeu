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

static int *pgm_argc = NULL;
static char **pgm_argv = NULL;

static Obj_t *pf_loadarg (io_t *io, void *data);
static Obj_t *pf_pconfig (io_t *io, void *data);

static Obj_t *pf_call (io_t *io, void *data)
{
	return Obj_call(data, NULL, NULL);
}

static Obj_t *expr_argc (void *par, const ObjList_t *list)
{
	return int2Obj(*pgm_argc);
}

static Obj_t *expr_argv (void *par, const ObjList_t *list)
{
	Buf_vec.type = &Type_str;
	Buf_vec.data = pgm_argv;
	Buf_vec.dim = *pgm_argc;
	return NewObj(&Type_vec, &Buf_vec);
}

static ParseDef_t esh_pdef[] = {
	{ "argc", pf_call, expr_argc },
	{ "argv", pf_call, expr_argv },
	{ "pconfig", pf_pconfig, NULL },
	{ "loadarg", pf_loadarg, NULL },
};


static void f_shift (Func_t *func, void *rval, void **arg);

int EshConfig (int *narg, char **arg)
{
	int i;
	CmdPar_t *par;

	SetFunc(0, &Type_void, "shift (int n = 1)", f_shift);
	AddParseDef(esh_pdef, tabsize(esh_pdef));

	pgm_argc = narg;
	pgm_argv = arg;

	if	(*pgm_argc <= 1 || pgm_argv[1][0] == '-')
		return 0;

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

	par = CmdPar_ptr(NULL);
	memfree(par->name);
	par->name = mstrcpy(ProgName);
	pconfig_init();
	return 1;
}


/*	Makrodefinitionen
*/

static int pconfig_flag = 0;

static Obj_t *pf_loadarg (io_t *io, void *data)
{
	io_eat(io, "%s");

	if	(!pconfig_flag)
	{
		loadarg(pgm_argc, pgm_argv);
		pconfig_exit();
	}
	else	io_message(io, MSG_EFMAIN, 9, 0); 

	return NULL;
}

static Obj_t *pf_pconfig (io_t *io, void *data)
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
	pconfig_flag = 1;
	pconfig_exit();
	return NULL;
}


/*	Funktionen
*/

static void f_shift (Func_t *func, void *rval, void **arg)
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
