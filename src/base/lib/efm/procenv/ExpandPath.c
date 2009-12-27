/*
Pfadnamen expandieren

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

#include <EFEU/procenv.h>
#include <EFEU/strbuf.h>
#include <EFEU/extension.h>
#include <ctype.h>

#if	HAS_GETPWNAM
#include <pwd.h>
#endif

/*
Die Funktion |$1| führt Tilde-expansion an einem Pfadnamen durch.
Eine Tilde |~| gefolgt von einem Benutzernamen (1. Zeichen ein Buchstabe,
danach beliebige Zeichen bis zu einem Schrägstrich |/|)
wird durch das HOME-Verzeichnis des Benutzers ersetzt.
Eine Tilde vor einem nichtalphabetischen Zeichen wurd durch den Wert
der Umgebungsvariablen |HOME| ersetzt.
*/

char *ExpandPath (const char *name)
{
	StrBuf *sb;

	if	(name == NULL)	return NULL;

	sb = sb_create(0);

	while (*name != 0)
	{
		if	(*name == '~')
		{
#if	HAS_GETPWNAM
			if	(isalpha(name[1]))
			{
				int save;
				struct passwd *pw;

				save = sb_getpos(sb);

				while (*(++name) != 0)
				{
					if	(*name == '/')
					{
						break;
					}
					else	sb_putc(*name, sb);
				}

				sb_putc(0, sb);
				pw = getpwnam((char *) sb->data + save);
				sb_setpos(sb, save);
				sb_sync(sb);

				if	(pw)
				{
					sb_puts(pw->pw_dir, sb);
				}

				continue;
			}
#endif
			sb_puts(getenv("HOME"), sb);
		}
		else	sb_putc(*name, sb);

		name++;
	}
	
	return sb2str(sb);
}
