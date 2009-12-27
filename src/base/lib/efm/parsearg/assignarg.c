/*
Zuweisungsargument bestimmen

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


#include <EFEU/parsearg.h>
#include <EFEU/memalloc.h>
#include <ctype.h>


AssignArg *assignarg (const char *arg, char **ptr, const char *delim)
{
	AssignArg *x;
	char *p;
	size_t n, po, pe, pa;
	int depth;

	if	(ptr)	*ptr = NULL;

	if	(arg == NULL)	return NULL;

	while (isspace(*arg))
		arg++;

	depth = 0;
	po = pe = pa = 0;

	for (n = 0; arg[n] != 0; n++)
	{
		if	(depth)
		{
			if	(arg[n] == '[')
			{
				depth++;
			}
			else if	(arg[n] == ']')
			{
				depth--;

				if	(depth == 0)	pe = n;
			}
		}
		else if	(delim && strchr(delim, arg[n]))
		{
			if	(ptr)
			{
				*ptr = (char *) arg + n;

				while (**ptr == ' ')
					(*ptr)++;

				if	(**ptr && strchr(delim, **ptr))
					(*ptr)++;
			}

			break;
		}
		else if	(pa)
		{
			;
		}
		else if	(arg[n] == '[')
		{
			po = n + 1;
			depth = 1;
		}
		else if	(arg[n] == '=')
		{
			pa = n + 1;
		}
	}

	x = (AssignArg *) memalloc(sizeof(AssignArg) + n + 1);
	p = strncpy((char *) (x + 1), arg, n);
	p[n] = 0;
	x->name = p;

	if	(po)
	{
		p[po - 1] = 0;
		p[pe] = 0;
		x->opt = p + po;
	}
	else	x->opt = NULL;

	if	(pa)
	{
		p[pa - 1] = 0;
		x->arg = p + pa;
	}
	else	x->arg = NULL;

	return x;
}
