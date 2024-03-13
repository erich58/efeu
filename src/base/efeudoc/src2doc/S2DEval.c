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

static  struct {
	char *name;
	S2DEval eval;
	char *desc;
} mode_tab[] = {
	{ "hdr", s2d_hdr, "Handbucheintrag für Headerfile"},
	{ "xhdr", s2d_xhdr, "Erweiterter Handbucheintrag für Headerfile"},
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

static struct {
	char *pat;
	S2DEval eval;
} name_tab[] = {
	{ "*.h", s2d_hdr },
	{ "*.pph", s2d_hdr },
	{ "*.c", s2d_src },
	{ "*.doc", s2d_doc },
	{ "*.eds", s2d_doc },
	{ "*.ms", s2d_cmd },
	{ "*.hdr", s2d_cmd },
	{ "*.smh", s2d_sh },
	{ "*.[0-9]*", s2d_man },
	{ "*Imakefile", s2d_std },
	{ "*.imake", s2d_xstd },
	{ "*Config.make", s2d_script },
};

S2DEval S2DEval_name (const char *name)
{
	int i;

	for (i = 0; i < tabsize(name_tab); i++)
		if (patcmp(name_tab[i].pat, name, NULL))
			return name_tab[i].eval;

	return NULL;
}


S2DEval S2DEval_mode (const char *name)
{
	int i;

	for (i = 0; i < tabsize(mode_tab); i++)
		if (mstrcmp(name, mode_tab[i].name) == 0)
			return mode_tab[i].eval;

	return NULL;
}

void S2DEval_list (IO *io)
{
	int i, j, k;

	for (i = 0; i < tabsize(mode_tab); i++)
	{
		io_xprintf(io, "%s\t%s\n", mode_tab[i].name, mode_tab[i].desc);

		for (j = k = 0; j < tabsize(name_tab); j++)
		{
			if	(mode_tab[i].eval == name_tab[j].eval)
			{
				io_puts(k ? ", " : "\t", io);
				io_puts(name_tab[j].pat, io);
				k++;
			}
		}

		if	(k)	io_putc('\n', io);
	}
}
