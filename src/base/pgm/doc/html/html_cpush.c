/*
Ausgabebefehle zwischenspeichern

$Copyright (C) 1999 Erich Frühstück
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

#include <HTML.h>
#include <efeudoc.h>
#include <ctype.h>

typedef struct {
	HTMLCmd cmd;
	void *data;
} ENTRY;

static ALLOCTAB(entry_tab, "HTMLCmd-Stack", 32, sizeof(ENTRY));

void HTMLCMD_note (HTML *html, void *data)
{
	HTML_rem(html, data);
}

void HTML_cpush (HTML *html, HTMLCmd cmd, void *data)
{
	ENTRY *entry = new_data(&entry_tab);
	entry->cmd = cmd;
	entry->data = data;
	pushstack(&html->s_cmd, entry);
}

void HTML_cpop (HTML *html)
{
	ENTRY *entry = popstack(&html->s_cmd, NULL);

	if	(entry)
	{
		if	(entry->cmd)
			entry->cmd(html, entry->data);

		del_data(&entry_tab, entry);
	}
}
