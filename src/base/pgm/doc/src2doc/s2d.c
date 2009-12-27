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
	S2DEval eval;
	char *desc;
} S2DMode;

static S2DMode tab[] = {
	{ "hdr", s2d_hdr, "Handbucheintrag für Headerfile"},
	{ "src", s2d_src, "Handbucheintrag für Sourcefile"},
	{ "cmd", s2d_cmd, "Handbucheintrag für Scriptdateien"},
	{ "sh", s2d_sh, "Handbucheintrag für Shellfunktionen"},
	{ "std", s2d_std, "C-Style Sourcen mit Erläuterung"},
	{ "xstd", s2d_xstd,
		"C-Style Sourcen mit Erläuterung ohne Kopfdefinition"},
	{ "script", s2d_script, "sh-Style Sourcen mit Erläuterung"},
	{ "xscript", s2d_xscript,
		"sh-Style Sourcen mit Erläuterung ohne Kopfdefinition"},
	{ "com", s2d_com, "Kommentare der Datei"},
	{ "doc", s2d_doc, "Direkteinbindung der Datei"},
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
	{ "*.smh", s2d_sh },
	{ "*.[0-9]*", s2d_man },
	{ "*Imakefile", s2d_std },
	{ "*.imake", s2d_xstd },
	{ "*Config.make", s2d_script },
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
