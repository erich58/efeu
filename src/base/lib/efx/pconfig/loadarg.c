/*
Laden von Aufrufparametern

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/pconfig.h>
#include <EFEU/Info.h>
#include <EFEU/RegExp.h>
#include <EFEU/Resource.h>
#include <EFEU/Debug.h>

#define	ARGNAME		"name"
#define	ARGVALUE	"arg"

#define LBL_OPT	":*:options" \
		":de:Optionen"

#define LBL_ARG	":*:arguments" \
		":de:Argumente"

#define	LBL_ENV	":*:environment" \
		":de:Umgebungsvariablen"

static void LoadOpt (int *argc, char **argv);
static void do_eval (const char *cmd, char *name, const char *arg);
static int x_eval (vecbuf_t *vb, char *name);
static int arg_eval (vecbuf_t *vb);

static int *argc;
static char **argv;

static char *argname = NULL;
static char *argvalue = NULL;

static VarDef_t vartab[] = {
	{ ARGNAME, &Type_str, &argname },
	{ ARGVALUE, &Type_str, &argvalue },
};

extern char *UsageFmt;

static int x_eval(vecbuf_t *vb, char *name)
{
	pardef_t *def;
	size_t n;

	for (n = vb->used, def = vb->data; n > 0; n--, def++)
	{
		if	(def->cmd && patcmp(def->name, name, NULL))
		{
			do_eval(def->cmd, mstrcpy(def->name), name);
			return 1;
		}
	}

	return 0;
}


/*	Programmparameter laden
*/

void loadarg(int *narg, char **arg)
{
	int flag;
/*
	VarStack_t *vstack = SaveVarStack();
*/

	argc = narg;
	argv = arg;

	PushVarTab(NULL, NULL);
	AddVarDef(NULL, vartab, tabsize(vartab));
/*
	PushVarTab(RefVarTab(GlobalVar), NULL);
*/
	AddInfo(NULL, "Opt", LBL_OPT, ArgInfo, &PgmDefTab.opt);
	AddInfo(NULL, "Arg", LBL_ARG, ArgInfo, &PgmDefTab.arg);
	AddInfo(NULL, "Env", LBL_ENV, ArgInfo, &PgmDefTab.env);
	LoadOpt(argc, argv);

	if	(PgmDefTab.xarg.used)
	{
		int i;

		for (i = 1; i < *argc; )
		{
			if	(x_eval(&PgmDefTab.xarg, argv[i]))
			{
				skiparg(argc, argv, i);
			}
			else	i++;
		}
	}
	
	flag = arg_eval(&PgmDefTab.arg);
	PopVarTab();
/*
	RestoreVarStack(vstack);
*/

	if	(flag && *narg != 1)
	{
		usage(UsageFmt, ioerr);
		exit(EXIT_FAILURE);
	}

	DebugMode(GetResource("Debug", NULL));
}

int arg_eval (vecbuf_t *buf)
{
	pardef_t *def;
	size_t n;
	int i;

	for (n = buf->used, def = buf->data; n > 0; n--, def++)
	{
		if	(def->flag == P_BREAK)
			return 0;

		if	(def->cmd == NULL)
			continue;

		if	(def->flag == P_REGEX)
		{
			RegExp_t *expr = RegExp(def->key, 0);

			for (i = 1; i < *argc; )
			{
				if	(RegExp_exec(expr, argv[i], NULL))
				{
					do_eval(def->cmd, NULL, argv[i]);
					skiparg(argc, argv, i);
				}
				else	i++;
			}

			rd_deref(expr);
		}
		else if	(*argc > 1)
		{
			do_eval(def->cmd, mstrcpy(def->name), argv[1]);
			skiparg(argc, argv, 1);
		}
		else if	(def->flag == P_ARG)
		{
			usage(UsageFmt, ioerr);
			exit(EXIT_FAILURE);
		}
	}

	return 1;
}


static int cmp_opt (const pardef_t *a, const pardef_t *b);

static xtab_t *opt_tab = NULL;

static int cmp_opt(const pardef_t *pa, const pardef_t *pb)
{
	register uchar_t *a;
	register uchar_t *b;

	a = (uchar_t *) pa->name;
	b = (uchar_t *) pb->name;

	if	(a == b)	return 0;
	if	(a == NULL)	return -1;
	if	(b == NULL)	return 1;

	for (; *a || *b; a++, b++)
	{
		if	(*a == *b)
		{
			if	(*a == ' ' || *a == '=')
			{
				return 0;
			}
			else	continue;
		}

		if	(*a == ' ')	return 1;
		if	(*b == ' ')	return -1;
		if	(*a == 0)	return 1;
		if	(*b == 0)	return -1;
		if	(*a < *b)	return -1;
		if	(*a > *b)	return 1;
	}

	return 0;
}




static int get_option (char *name, char *arg);


/*	Programmparameter laden
*/

static void LoadOpt(int *argc, char **argv)
{
	pardef_t *p;
	int i;

	opt_tab = xcreate(0, (comp_t) cmp_opt);

	for (i = PgmDefTab.opt.used, p = PgmDefTab.opt.data; i > 0; i--, p++)
		if (p->name && p->cmd) xsearch(opt_tab, p, XS_REPLACE);

/*	Optionen abfragen
*/
	for (i = 1; i < *argc; )
	{
		if	(argv[i][0] != '-' || argv[i][1] == 0)
		{
			i++;
			continue;
		}
		else if	(argv[i][1] == '-' && argv[i][2] == 0)
		{
			skiparg(argc, argv, i);
			break;
		}

		if	(x_eval(&PgmDefTab.xopt, argv[i] + 1))
		{
			skiparg(argc, argv, i);
			continue;
		}

		switch (get_option(argv[i] + 1, (i + 1 < *argc ? argv[i + 1] : NULL)))
		{
		case -1:
			reg_cpy(1, argv[i]);
			liberror(MSG_EFMAIN, 31);
			break;
		case 0:
			reg_cpy(1, argv[i]);
			liberror(MSG_EFMAIN, 32);
			break;
		case 2:
			skiparg(argc, argv, i + 1);
			/* FALLTHROUGH */
		case 1:
			skiparg(argc, argv, i);
			break;
		}
	}

	xdestroy(opt_tab, NULL);
}


static int get_option(char *name, char *arg)
{
	int i, j;

	if	(opt_tab == NULL)	return 0;

	for (i = 0; i < opt_tab->dim; i++)
	{
		pardef_t *opt;
		int flag;

		opt = opt_tab->tab[i];
		j = 0;
		flag = 3;

		do
		{
			if	(opt->name[j] == ' ')	
			{
				flag = 2;
				name += j;
			}
			else if	(opt->name[j] != name[j])
			{
				flag = 0;
			}
			else if (opt->name[j] == '=')	
			{
				flag = 2;
				name += j + 1;
			}
			else if	(opt->name[j] == 0)	
			{
				flag = 1;
			}
			else	j++;
		}
		while	(flag == 3);

		if	(flag == 2)
		{
			if	(*name != 0)
			{
				arg = name;
				flag = 1;
			}
			else if	(arg == NULL)
			{
				return -1;
			}
		}
		else if	(flag != 1)
		{
			continue;
		}

		do_eval(opt->cmd, mstrncpy(opt->name, j), arg);
		return flag;
	}

	return 0;
}



static void do_eval (const char *cmd, char *name, const char *arg)
{
	argname = name;
	argvalue = argval(arg);
	reg_cpy(1, argvalue);	/* Wegen Kompatiblität */
	streval(cmd);
	memfree(argname);
	memfree(argvalue);
	argname = NULL;
	argvalue = NULL;
}

