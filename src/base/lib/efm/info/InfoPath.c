/*
Suchpfad für Informationsdateien

$Copyright (C) 2001 Erich Frühstück
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
#include <EFEU/Info.h>
#include <EFEU/LangDef.h>

#define	_String(x)	#x
#define	String(x)	_String(x)
#define	TOP		String(EFEUROOT)

/*
Die Variable |$1| bestimmt den Suchpfad für Informationsdateien.
*/

char *InfoPath = NULL;

/*
Die Funktion |$1| initialisiert den Suchpfad |InfoPath| für Informationsdateien.
Falls für <path> ein Nullpointer angegeben wurde und der Suchpfad bereits
initialisiert wurde, wird er von |$1| nicht verändert.
*/

void SetInfoPath (const char *path)
{
	StrBuf *sb;

	if	(InfoPath && !path)	return;

	sb = new_strbuf(0);

/*	Basispfade
*/
#if	0
	sb_putc('.', sb);
#endif

	if	(path)
	{
		if	(sb_getpos(sb))	sb_putc(':', sb);

		sb_puts(path, sb);
	}

/*	Systemsuchpfade
*/
	if	(sb_getpos(sb))	sb_putc(':', sb);

	sb_printf(sb, "%s/lib/eis/%%L/", TOP);
	sb_printf(sb, ":%s/lib/efeu/%%L/info", TOP);

	memfree(InfoPath);
	InfoPath = sb2str(sb);
}
