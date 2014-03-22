/*
:*:	initializing Gtk+
:de:	Gtk+ initialisieren

$Copyright (C) 2001 Erich Frühstück
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

#include <GUI/EGtk.h>
#include <GUI/EGtkWidget.h>
#include <EFEU/CmdPar.h>
#include <EFEU/object.h>
#include <locale.h>

#define	SET_LOCALE	1
#define	ARG_BSIZE	32

#if	HAS_GTK
static int arg_size = 0;
static int arg_dim = 0;
static char **arg_data = NULL;

static void arg_add (char *arg)
{
	if	(arg_dim + 1 >= arg_size)
	{
		char **save = arg_data;
		arg_size += ARG_BSIZE;
		arg_data = lmalloc(arg_size * sizeof(char *));
		memcpy(arg_data, save, arg_dim * sizeof(char *));
	}

	arg_data[arg_dim++] = arg;
	arg_data[arg_dim] = NULL;
}

static void arg_xadd (char *arg)
{
	if	(arg)	
	{
		arg_add(arg);
		arg = (*arg == '-') ? strchr(arg, ' ') : NULL;

		if	(arg)
		{
			*arg = 0;
			arg_add(arg + 1);
		}
	}
	else	arg_add("");
}

static int e_gtkarg (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	arg_xadd(CmdPar_psub(cpar, par, arg));
	return 0;
}

static CmdParEval edef = { "gtkarg", "GTK-Argument setzen", e_gtkarg };

#if	!SET_LOCALE
static void f_gtklocale (EfiFunc *func, void *rval, void **arg)
{
	gtk_set_locale();
}
#endif

static void f_gtkarg (EfiFunc *func, void *rval, void **arg)
{
	arg_xadd(mstrcpy(Val_str(arg[0])));
}

static EfiFuncDef fdef[] = {
#if	!SET_LOCALE
	{ 0, &Type_void, "gtklocale()", f_gtklocale },
#endif
	{ 0, &Type_void, "gtkarg(str arg)", f_gtkarg },
};

void EGtkRes (const char *arg)
{
	static int need_setup = 1;

	if	(need_setup)
	{
		CmdParEval_add(&edef);
		AddFuncDef(fdef, tabsize(fdef));
		CmdPar_load(NULL, "EGtk", 0);
		need_setup = 0;
	}

	arg_add(mstrcpy(arg));
}

void EGtkInit (void)
{
	static int need_init = 1;

	if	(need_init)
	{
#if	SET_LOCALE
		gtk_set_locale();
		setlocale(LC_NUMERIC, "C");
#endif
		need_init = 0;
		gtk_init(&arg_dim, &arg_data);
		EGtkObject_setup();
	}
}

#endif

