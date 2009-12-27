/*
Formatierter Parser für Objekte

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

#include <EFEU/object.h>
#include <EFEU/Op.h>

#define	PROMPT	">>> "

/* VARARGS 2 */

int Parse_fmt(io_t *io, const char *fmt, ...)
{
	int c;
	va_list k;
	int err;
	char *prompt;

	if	(fmt == NULL)	return 0;

	va_start(k, fmt);
	prompt = io_prompt(io, PROMPT);

	for (err = 0; !err && *fmt; fmt++)
	{
		switch (*fmt)
		{
		case 'T':	*va_arg(k, Obj_t **) = Parse_term(io, 0); break;
		case 'A':	*va_arg(k, Obj_t **) = Parse_term(io, OpPrior_Comma); break;
		case 'C':	*va_arg(k, Obj_t **) = Parse_cmd(io); break;
		case ' ':	io_eat(io, "%s"); break;

		default:

			if	((c = io_eat(io, " \t")) != *fmt)
			{
				reg_fmt(1, "%#c", c);
				reg_fmt(2, "%#c", *fmt);
				err = 109;
			}
			else	io_getc(io);

			break;
		}
	}

	io_prompt(io, prompt);
	va_end(k);

	if	(err)
	{
		io_error(io, MSG_EFMAIN, err, 2, reg_get(1), reg_get(2));
		return 0;
	}

	return 1;
}
