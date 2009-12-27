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
#include <EFEU/mstring.h>
#include <ctype.h>
#include <EFEU/io.h>


static int subcopy (const char *arg, int delim)
{
	int n;

	for (n = 1; arg[n] != delim; n++)
	{
		switch (arg[n])
		{
		case  0:	return n;
		case '{':	n += subcopy(arg + n, '}'); break;
		case '[':	n += subcopy(arg + n, ']'); break;
		case '(':	n += subcopy(arg + n, ')'); break;
		default:	break;
		}
	}

	return n;
}

AssignArg *assignarg (const char *arg, char **ptr, const char *delim)
{
	AssignArg *x;
	char *p;
	size_t n, po, pe, pa;
	int trim_arg;

	if	(ptr)	*ptr = NULL;

	if	(arg == NULL)	return NULL;

	while (isspace(*arg))
		arg++;

	if	(*arg == 0)	return NULL;

	if	(*arg == '{')
	{
		x = assignarg(arg + 1, ptr, "}");

		if	(ptr && *ptr && **ptr && delim && strchr(delim, **ptr))
		{
			(*ptr)++;

			while (isspace(**ptr))
				(*ptr)++;
			
			if	(**ptr && strchr(delim, **ptr))
				(*ptr)++;
		}

		return x;
	}

	po = pe = pa = 0;
	trim_arg = 0;

	for (n = 0; arg[n] != 0; n++)
	{
		if	(delim && strchr(delim, arg[n]))
		{
			if	(ptr)
			{
				*ptr = (char *) arg + n;

				while (isspace(**ptr))
					(*ptr)++;

				if	(**ptr && strchr(delim, **ptr))
					(*ptr)++;
			}

			break;
		}
		else if	(pa)
		{
			switch (arg[n])
			{
			case '{':	n += subcopy(arg + n, '}'); break;
			case '[':	n += subcopy(arg + n, ']'); break;
			case '(':	n += subcopy(arg + n, ')'); break;
			default:	break;
			}
		}
		else if	(arg[n] == '[')
		{
			po = n + 1;
			n += subcopy(arg + n, ']');
			pe = n;
		}
		else if	(arg[n] == '{')
		{
			n += subcopy(arg + n, '}');
		}
		else if	(arg[n] == '=')
		{
			pa = n + 1;
		}
		else if	(isspace((unsigned char) arg[n]) && !delim)
		{
			pa = n + 1;
			trim_arg = 1;
		}
		else	po = pe = 0;
	}

	x = memalloc(sizeof *x + n + 1);
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

		if	(trim_arg)
			mtrim(x->arg);
	}
	else	x->arg = NULL;

	return x;
}
