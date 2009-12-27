/*
Kommandoparameter in Datei sichern

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

#include <EFEU/CmdPar.h>
#include <EFEU/ftools.h>
#include <EFEU/procenv.h>
#include <EFEU/parsearg.h>
#include <EFEU/RegExp.h>
#include <EFEU/parsub.h>

#define	MATCH_PART	1
#define	MATCH_FULL	2

static char *get_arg (const char *arg)
{
	if	(arg == NULL)
		return NULL;

	if	(arg[0] == '-' && arg[1] == 0)
		return NULL;

	if	(arg[0] == '\\')
		arg++;

	return (char *) arg;
}

static int cmp_key (const char *a, const char *b, const char **ptr)
{
	do
	{
		if	(*a != *b)	return 0;

		a++;
		b++;
	}
	while	(*a && *b && *b != '=');

	*ptr = *b ? b : NULL;
	return (*a || (*b && *b != '=')) ? MATCH_PART : MATCH_FULL;
}

static CmdParKey_t *get_key (CmdPar_t *par, const char *name, const char **ptr)
{
	CmdParKey_t **kp, *key;
	size_t n;

	key = NULL;
	*ptr = NULL;

	for (n = par->opt.used, kp = par->opt.data; n-- > 0; kp++)
	{
		switch	(cmp_key((*kp)->key, name, ptr))
		{
		case MATCH_FULL:
			return *kp;
		case MATCH_PART:
			if	(key)
			{
				message(NULL, MSG_EFM, 33, 3,
					name, key->key, (*kp)->key);
				return NULL;
			}

			key = *kp;
		default:
			break;
		}
	}

	if	(key == NULL)
		message(NULL, MSG_EFM, 32, 1, name);

	return key;
}

static int get_option (CmdPar_t *par, int *narg, char **arg, int i)
{
	CmdParKey_t *key;
	const char *opt;
	const char *cmdarg;
	int flag, rval, lopt;

	opt = arg[i] + 1;
	flag = 0;

	while (opt)
	{
		key = get_key(par, opt, &opt);

		if	(key == NULL)
			return CMDPAR_ERR;

		lopt = key->key[1] ? 1 : 0;

		if	(flag && lopt)
		{
			message(NULL, MSG_EFM, 34, 1, arg[i]);
			return CMDPAR_ERR;
		}

	/*	Option ohne Argument:
	*/
		if	(key->argtype == ARGTYPE_NONE)
		{
			if	(lopt && opt)
			{
				message(NULL, MSG_EFM, 34, 1, arg[i]);
				return CMDPAR_ERR;
			}

			cmdarg = key->val;
			rval = CmdParDef_eval(par, key->def, cmdarg);

			if	(lopt || (rval & CMDPAR_END))
				return rval;

			flag = 1;
			continue;
		}

	/*	Argument bestimmen
	*/
		if	(opt == NULL)
		{
			if	(key->argtype == ARGTYPE_STD)
			{
				if	(i + 1 >= *narg)
				{
					message(NULL, MSG_EFM, 31, 1, key->key);
					return CMDPAR_ERR;
				}

				cmdarg = arg[i + 1];
				skiparg(narg, arg, i + 1);
			}
			else	cmdarg = NULL;
		}
		else if	(lopt)
		{
			if	(*opt != '=')
			{
				message(NULL, MSG_EFM, 34, 1, arg[i]);
				return CMDPAR_ERR;
			}

			cmdarg = get_arg(opt + 1);
		}
		else	cmdarg = get_arg(opt);

		if	(cmdarg == NULL)
			cmdarg = (const char *) key->val;

		return CmdParDef_eval(par, key->def, cmdarg);
	}

	return 0;
}

#define	RMARG(x,m)	mstrncpy((x) + (m).rm_so, (m).rm_eo - (m).rm_so)

static int regex_arg(CmdPar_t *par, CmdParKey_t *key,
	int *narg, char **arg, int pos, int *end)
{
	const char *cmdarg;
	char *buf;
	regmatch_t *rm;
	RegExp_t *expr;
	int k, rval;

	expr = RegExp(key->key, 0);
	rval = 0;

	while (pos < *end)
	{
		k = RegExp_exec(expr, arg[pos], &rm);
		buf = NULL;

		switch (k)
		{
		case 0:
			pos++;
			continue;
		case 1:
			cmdarg = arg[pos];
			break;
		default:
			buf = RMARG(arg[pos], rm[1]);
			cmdarg = buf;
			break;
		}

		rval = CmdParDef_eval(par, key->def, cmdarg);
		memfree(buf);
		skiparg(narg, arg, pos);
		(*end)--;

		if	(rval & CMDPAR_END)	break;
	}

	rd_deref(expr);
	return rval;
}

/*
Die Funktion |$1| wertet die Befehlsargumente entsprechend der
Parameterstruktur |CmdPar_t| aus. Die Befehlsargumente
werden abhängig von der Parameterdefinitionen entsprechend
modifiziert. Falls <flag> verschieden von 0 ist, wird die
Optionsabfrage beim ersten Argument, dass nicht mit einer
Optionskennung beginnt, gestoppt.

Die Funktion |$1| liefert die Zahl der
verbleibenden Argumente (größer gleich 1), -1 bei einem
Fehler und 0 bei einem Abbruch der Argumentabfrage.
*/

int CmdPar_eval (CmdPar_t *par, int *narg, char **arg, int flag)
{
	CmdParKey_t **key;
	int i, rval, start, end;
	size_t n;

	par = CmdPar_ptr(par);

/*	Umgebungsvariablen
*/
	for (n = par->env.used, key = par->env.data; n-- > 0; key++)
	{
		const char *value = getenv((*key)->key);

		if	(value)
			rval = CmdParDef_eval(par, (*key)->def, value);
		else if	((*key)->val)
			rval = CmdParDef_eval(par, (*key)->def, (*key)->val);
		else	continue;

		if	(rval & CMDPAR_ERR)	return -1;
		if	(rval & CMDPAR_BREAK)	return 0;
	}

/*	Optionen abfragen
*/
	for (i = 1; i < *narg; )
	{
		if	(arg[i][0] != '-' || arg[i][1] == 0)
		{
			i++;

			if	(flag)	break;
			continue;
		}

		if	(arg[i][1] == '-' && arg[i][2] == 0)
		{
			skiparg(narg, arg, i);
			break;
		}

		if	(arg[i][1] == '?' && arg[i][2] == 0)
		{
			CmdPar_usage(par, NULL, NULL);
			exit(EXIT_SUCCESS);
			break;
		}

		rval = get_option(par, narg, arg, i);
		skiparg(narg, arg, i);

		if	(rval & CMDPAR_ERR)	return -1;
		if	(rval & CMDPAR_BREAK)	return 0;
	}

/*	Argumente
*/
	start = 1;
	end = *narg;

	for (n = par->arg.used, key = par->arg.data; n-- > 0; key++)
	{
		int pos = start;
		int dim = 1;
		const char *cmdarg;

		switch ((*key)->argtype)
		{
		case ARGTYPE_STD:
			dim = (start < end) ? 1 : -1;
			break;
		case ARGTYPE_OPT:
			dim = (start < end) ? 1 : 0;
			break;
		case ARGTYPE_LAST:
			pos = end - 1;
			dim = (start < end) ? 1 : -1;
			break;
		case ARGTYPE_VA0:
			dim = end - start;
			break;
		case ARGTYPE_VA1:
			dim = (start < end) ? end - start : -1;
			break;
		case ARGTYPE_REGEX:
			rval = regex_arg(par, *key, narg, arg, start, &end);

			if	(rval & CMDPAR_ERR)	return -1;
			if	(rval & CMDPAR_BREAK)	return 0;

			continue;
		}

		if	(dim < 0)
		{
			CmdPar_usage(par, NULL, NULL);
			return 0;
		}

		while (dim-- > 0)
		{
			cmdarg = arg[pos];

			if	((cmdarg[0] == '-' && cmdarg[1] == 0))
				cmdarg = (*key)->val;

			rval = CmdParDef_eval(par, (*key)->def, cmdarg);

			if	(rval & CMDPAR_KEEP)
			{
				if	(pos == start)
				{
					start++;
					pos = start;
				}
				else	end--;
			}
			else
			{
				end--;
				skiparg(narg, arg, pos);
			}

			if	(rval & CMDPAR_ERR)	return -1;
			if	(rval & CMDPAR_BREAK)	return 0;
		}
	}

	if	(start < end)
	{
		CmdPar_usage(par, NULL, NULL);
		return -1;
	}

	return *narg;
}

/*
$SeeAlso
\mref{CmdPar(3)},
\mref{CmdParCall(3)},
\mref{CmdParDef(3)},
\mref{CmdParEval(3)},
\mref{CmdParExpand(3)},
\mref{CmdParKey(3)},
\mref{CmdParVar(3)},
\mref{CmdPar_list(3)},
\mref{CmdPar_load(3)},
\mref{CmdPar_psub(3)},
\mref{CmdPar_usage(3)},
\mref{CmdPar_write(3)},
\mref{Resource(3)},
\mref{CmdPar(7)}.
*/

