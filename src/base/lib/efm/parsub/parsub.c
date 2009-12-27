/*
Parametersubstitution

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

#include <EFEU/vecbuf.h>
#include <EFEU/parsub.h>
#include <ctype.h>

int psub_key = '$';

/*	Tabelle mit Substitutionsfunktionen
*/

static VECBUF(psubtab, 32, sizeof(copydef_t));

static int pdef_cmp(copydef_t *a, copydef_t *b)
{
	return (b->key - a->key);
}

void psubfunc(int key, iocopy_t copy, void *par)
{
	copydef_t cdef;

	cdef.key = key;
	cdef.copy = copy;
	cdef.par = par;
	vb_search(&psubtab, &cdef, (comp_t) pdef_cmp, copy ? VB_REPLACE : VB_DELETE);
}


/*	Kopierfunktion
*/

int iocpy_psub(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	copydef_t def, *ptr;

	def.key = io_getc(in);
	ptr = vb_search(&psubtab, &def, (comp_t) pdef_cmp, VB_SEARCH);

	if	(ptr && ptr->copy)
	{
		return ptr->copy(in, out, ptr->par);
	}
	else if	(isdigit(def.key))
	{
		char *arg = reg_get(def.key - '0');
		return flags ? io_xputs(arg, out, "\"") : io_puts(arg, out);
	}
	else	return (io_putc(def.key, out) != EOF) ? 1 : 0;
}


int io_pcopy(io_t *in, io_t *out, void *arg)
{
	int c, n, flag;

	n = 0;
	flag = 0;

	while ((c = io_mgetc(in, 1)) != EOF)
	{
		if	(c == psub_key)
		{
			n += iocpy_psub(in, out, c, NULL, flag);
		}
		else
		{
			if	(c == '"')	flag = !flag;

			n += io_nputc(c, out, 1);
		}
	}

	return n;
}

char *mpcopy(io_t *in)
{
	return miocopy(in, io_pcopy, NULL);
}


int io_psub(io_t *out, const char *str)
{
	return iocopystr(str, out, io_pcopy, NULL);
}


char *parsub(const char *str)
{
	return miocopystr(str, io_pcopy, NULL);
}
