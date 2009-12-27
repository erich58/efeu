/*
Dokumentmakros auflisten

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

#include <efeudoc.h>

static void mac_add (InfoNode *info, DocMac *mac)
{
	InfoNode *x = AddInfo(info, mac->name, NULL, NULL,
		msprintf("%s\n---- verbatim\n%s//END\n----\n",
			mac->desc, mac->fmt));

	if	(mac->desc)
	{
		char *p = strchr(mac->desc, '\n');

		if	(p)
			x->label = mstrncpy(mac->desc, p - mac->desc);
	}
}

static void mac_load (InfoNode *info)
{
	info->par = NULL;

	if	(GlobalDocTab)
	{
		size_t n = GlobalDocTab->mtab.used;
		DocMac *mp = GlobalDocTab->mtab.data;

		while (n-- > 0)
			mac_add(info, mp++);
	}
}

void DocMacInfo (const char *name, const char *desc)
{
	InfoNode *info = AddInfo(NULL, name, desc, NULL, NULL);
	info->setup = mac_load;
}
