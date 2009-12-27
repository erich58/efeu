/*
Befehlszeile parsen

$Copyright (C) 1993 Erich Frühstück
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


static iocpy_t cdef[] = {
	{ " \t\n;", NULL, 0, NULL },
	{ "/", NULL, 1, iocpy_cskip },
	{ "\\", "!", 0, iocpy_esc },
	{ "\"", "\"", 0, iocpy_xstr },
	{ "'", "'", 0, iocpy_str },
	{ "$", NULL, 0, iocpy_psub },
};


int parse_line(io_t *io, char ***ptr)
{
	io_t *tmp;
	char *p;
	int c, i, dim;
	size_t size;

	if	(io == NULL || ptr == NULL)	return 0;
	if	(io_eat(io, "%s;") == EOF)	return 0;

/*	Zwischenspeichern der Argumente
*/
	tmp = io_tmpbuf(0);
	dim = 0;
	size = 0;

	while ((c = io_eat(io, " \t")) != EOF)
	{
		if	(c == '\n' || c == ';')
			break;

		io_putc(1, tmp);
		size += iocpy(io, tmp, cdef, tabsize(cdef));
		size += io_nputc(0, tmp, 1);
		io_putc(iocpy_flag, tmp);	/* Flag für Nullkennung */
		dim++;
	}

/*	Laden der Argumente
*/
	io_rewind(tmp);
	*ptr = malloc(dim * sizeof(char *) + size);
	p = (char *) ((*ptr) + dim);

	for (i = 0; i < dim; i++)
	{
		(*ptr)[i] = io_loadstr(tmp, &p);
		c = io_getc(tmp);

		if	(c == 0 && strcmp((*ptr)[i], "NULL") == 0)
			(*ptr)[i] = NULL;
	}

	io_close(tmp);
	return dim;
}
