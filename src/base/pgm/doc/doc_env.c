/*
Dokumenthilfsprogramme

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

#include "efeudoc.h"
#include <ctype.h>

#define	LIST_DEPTH	4

static ALLOCTAB(env_tab, 32, sizeof(DocEnv_t));

void Doc_newenv (Doc_t *doc, int depth, ...)
{
	DocEnv_t *env;
	va_list list;

	Doc_start(doc);

	if	(doc->env.pflag)
		io_putc('\n', doc->out);

	env = new_data(&env_tab);
	*env = doc->env;
	pushstack(&doc->env_stack, env);
	memset(&doc->env, 0, sizeof(DocEnv_t));
	doc->env.indent = doc->indent;
	doc->env.cpos = env->cpos;
	doc->env.depth = env->depth + depth;
	va_start(list, depth);
	doc->env.type = va_arg(list, int);
	va_end(list);
	va_start(list, depth);
	io_vctrl(doc->out, DOC_BEG, list);
	va_end(list);
}


void Doc_endenv (Doc_t *doc)
{
	DocEnv_t *env;

	if	(doc->env_stack)
	{
		Doc_par(doc);
		io_ctrl(doc->out, DOC_END, doc->env.type);
		memfree(doc->env.item);
		env = popstack(&doc->env_stack, NULL);
		doc->env = *env;
		doc->env.cpos = 0;
		del_data(&env_tab, env);
	}
}

static int testtype(int type, int mode)
{
	switch (type)
	{
	case DOC_ENV_MPAGE:
		return (mode < 1);
	default:
		break;
	}

	return 1;
}

void Doc_endall (Doc_t *doc, int mode)
{
	Doc_start(doc);

	while (doc->env_stack && testtype(doc->env.type, mode))
		Doc_endenv(doc);

	Doc_par(doc);
}

static void end_list (Doc_t *doc)
{
	Doc_endenv(doc);

	if	(doc->env.hmode)
		io_ctrl(doc->out, DOC_CMD, DOC_CMD_TEX,
			"\\vskip-\\baselineskip\n");
/*
	Doc_par(doc);
	Doc_hmode(doc);
*/
}

static void list_item (Doc_t *doc)
{
	doc->env.pflag = 0;

	if	(doc->env.item)
	{
		Doc_stdpar(doc, DOC_PAR_TAG);
		Doc_hmode(doc);
		Doc_scopy(doc, doc->env.item);
		Doc_par(doc);
		doc->env.pflag = 0;
	}
	else if	(io_ctrl(doc->out, DOC_CMD, DOC_CMD_ITEM) == EOF)
	{
		io_nputc(' ', doc->out, doc->env.depth - doc->env.cpos);
		io_putc('*', doc->out);
		doc->env.cpos = doc->env.depth + 1;
	}

	doc->env.par_beg = end_list;
	Doc_newenv(doc, LIST_DEPTH, DOC_ENV_BASE);
	Doc_hmode(doc);
}

int Doc_islist (Doc_t *doc)
{
	DocEnv_t *env;

	if	(DOC_IS_LIST(doc->env.type))	return 0;
	if	(doc->env_stack == NULL)	return 0;

	env = doc->env_stack->data;
	return (DOC_IS_LIST(env->type) && env->indent == doc->indent);
}

void Doc_item (Doc_t *doc, int type)
{
	if	(Doc_islist(doc))
		Doc_endenv(doc);

	if	(doc->indent > doc->env.indent)
	{
		Doc_newenv(doc, 0, type);
	}
	else if	(doc->env.type != type)
	{
		if	(DOC_IS_LIST(doc->env.type))
			Doc_endenv(doc);

		Doc_newenv(doc, 0, type);
	}
	else
	{
		memfree(doc->env.item);
		doc->env.item = NULL;
	}

	doc->env.par_beg = list_item;
}

void Doc_endlist (Doc_t *doc)
{
	while (doc->env_stack && !DOC_IS_LIST(doc->env.type))
		Doc_endenv(doc);

	Doc_endenv(doc);
}
