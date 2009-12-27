/*
Verarbeitungsmodi

$Copyright (C) 2000 Erich Frühstück
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

#include <EFEU/patcmp.h>
#include <EFEU/mstring.h>
#include "src2doc.h"

typedef struct {
	char *name;
	char *desc;
	S2DEval eval;
} S2DMode;

static S2DMode tab[] = {
	{ "hdr", "Handbucheintrag für Headerfile", s2d_hdr },
	{ "src", "Handbucheintrag für Sourcefile", s2d_src },
	{ "cmd", "Handbucheintrag für Scriptdateien", s2d_cmd },
	{ "std", "Sourcen mit Erläuterung", s2d_std },
	{ "com", "Kommentare der Datei", s2d_com },
	{ "doc", "Direkteinbindung der Datei", s2d_doc },
};

typedef struct {
	char *pat;
	S2DEval eval;
} S2DName;

static S2DName ntab[] = {
	{ "*.h", s2d_hdr },
	{ "*.pph", s2d_hdr },
	{ "*.c", s2d_src },
	{ "*.doc", s2d_doc },
	{ "*.ms", s2d_cmd },
	{ "*.hdr", s2d_cmd },
	{ "*.[0-9]*", s2d_man },
	{ "*Imakefile", s2d_std },
};

S2DEval S2DName_get (const char *name)
{
	int i;

	for (i = 0; i < tabsize(ntab); i++)
		if (patcmp(ntab[i].pat, name, NULL)) return ntab[i].eval;

	return NULL;
}


S2DEval S2DMode_get (const char *name)
{
	int i;

	for (i = 0; i < tabsize(tab); i++)
		if (mstrcmp(name, tab[i].name) == 0) return tab[i].eval;

	return NULL;
}

void S2DMode_list (IO *io)
{
	int i, j, k;

	for (i = 0; i < tabsize(tab); i++)
	{
		io_printf(io, "%s\t%s\n", tab[i].name, tab[i].desc);

		for (j = k = 0; j < tabsize(ntab); j++)
		{
			if	(tab[i].eval == ntab[j].eval)
			{
				io_puts(k ? ", " : "\t", io);
				io_puts(ntab[j].pat, io);
				k++;
			}
		}

		if	(k)	io_putc('\n', io);
	}
}
