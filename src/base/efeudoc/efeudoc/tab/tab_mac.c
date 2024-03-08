/*
Befehle abfragen/einfügen

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

void DocMac_clean (DocMac *mac)
{
	memfree(mac->name);
	memfree(mac->desc);
	memfree(mac->fmt);
}


void DocMac_print (IO *io, DocMac *mac, int mode)
{
	if	(io && mac && mac->desc)
	{
		io = io_refer(io);

		if	(mode & 0x1)
		{
			io_xprintf(io, "[|%s|]\n",  mac->name);
			io = io_lmark(io, "\t", NULL, 0);
		}

		io_psubarg(io, mac->desc, "nm", msprintf("|%s|", mac->name));

		if	(mode & 0x2)
		{
			io_nlputs("\n---- verbatim", io);
			io_nlputs(mac->fmt, io);
			io_nlputs("----", io);
		}

		io_close(io);
	}
}

static int mac_cmp (const void *pa, const void *pb)
{
	const DocMac *a = pa;
	const DocMac *b = pb;
	return mstrcmp(a->name, b->name);
}

DocMac *DocTab_getmac (DocTab *tab, const char *name)
{
	if	(tab)
	{
		DocMac key;
		key.name = (char *) name;
		return vb_search(&tab->mtab, &key, mac_cmp, VB_SEARCH);
	}
	else	return NULL;
}

void DocTab_setmac (DocTab *tab, char *name, char *desc, char *fmt)
{
	DocMac key;

	key.name = name;
	key.desc = desc;
	key.fmt = fmt;
	vb_search(&tab->mtab, &key, mac_cmp, VB_REPLACE);
	DocMac_clean(&key);
}
