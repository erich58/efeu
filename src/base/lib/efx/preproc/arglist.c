/*
Argumentliste generieren

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

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/preproc.h>
#include <EFEU/efutil.h>
#include <EFEU/vecbuf.h>

#define	PROMPT	"( >>> "

static iocpy_t copy_def[] = {
	{ "/", "\n", 1, iocpy_cskip },
	{ "\\", ",)", 0, iocpy_esc },
	{ "\\", "!", 1, iocpy_esc },
	{ "\"", "\"", 1, iocpy_str },
	{ "'",	"'", 1, iocpy_str },
	{ "(",	")", 1, iocpy_brace },
	{ "{",	"}", 1, iocpy_brace },
	{ "[",	"]", 1, iocpy_brace },
	{ ",)", NULL, 0, NULL },
	{ "%a_", "!%n_", 0, iocpy_macsub },
};


size_t macarglist(io_t *io, char ***ptr)
{
	vecbuf_t vb;
	char *prompt;
	char **p;
	int c;
	int dim;

	if	(ptr == NULL)
	{
		return 0;
	}
	else	*ptr = NULL;

	vb_init(&vb, 16, sizeof(char *));
	prompt = io_prompt(io, PROMPT);

/*	Argumente bestimmen
*/
	while ((c = io_eat(io, "%s")) != EOF)
	{
		p = vb_next(&vb);
		*p = NULL;

		if	(c != ',' && c != ')')
			*p = miocpy(io, copy_def, tabsize(copy_def));

		c = io_getc(io);

		if	(c == ')')	break;
	}

	io_prompt(io, prompt);

/*	Argumentliste übernehmen
*/
	dim = vb.used;
	*ptr = memalloc(dim * sizeof(char *));
	memcpy(*ptr, vb.data, dim * sizeof(char *));
	vb_free(&vb);
	return dim;
}
