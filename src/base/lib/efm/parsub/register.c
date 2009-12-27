/*
Register für Parametersubstitution

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

#include <EFEU/mstring.h>
#include <EFEU/parsub.h>

#define	REG_BLK_SIZE	8	/* Blokgröße für Register */

static char **reg_list = NULL;
static int max_reg = 0;


/*	Register setzen
*/

char *reg_set (int n, char *arg)
{
	if	(n >= max_reg)
	{
		int i;
		char **old_list;
		int old_dim;

		old_list = reg_list;
		old_dim = max_reg;
		max_reg = REG_BLK_SIZE * (1 + n / REG_BLK_SIZE);
		reg_list = memalloc(max_reg * sizeof(char *));

		for (i = 0; i < old_dim; i++)
			reg_list[i] = old_list[i];

		for (i = old_dim; i < max_reg; i++)
			reg_list[i] = NULL;

		memfree(old_list);
	}
	else if	(n < 0)
	{
		memfree(arg);
		return NULL;
	}
	else	memfree(reg_list[n]);

	return reg_list[n] = arg;
}


/*	Register abrufen
*/

char *reg_get (int n)
{
	if	(n >= 0 && n < max_reg)
	{
		return reg_list[n];
	}
	else	return NULL;
}


char *reg_cpy (int n, const char *arg)
{
	return reg_set(n, mstrcpy(arg));
}

char *reg_fmt (int num, const char *fmt, ...)
{
	va_list list;
	char *p;

	va_start(list, fmt);
	p = reg_set(num, mvsprintf(fmt, list));
	va_end(list);
	return p;
}

char *reg_str (int n, const char *arg)
{
	return reg_fmt(n, "%#s", arg);
}

void reg_clear (void)
{
	int n;

	for (n = 0; n < max_reg; n++)
	{
		memfree(reg_list[n]);
		reg_list[n] = NULL;
	}
}
