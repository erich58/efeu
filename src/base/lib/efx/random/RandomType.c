/*
Zufallsgeneratortype

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

#include <EFEU/Random.h>
#include <EFEU/mstring.h>
#include <EFEU/vecbuf.h>
#include <EFEU/Info.h>
#include <EFEU/procenv.h>

static VECBUF(TypeTab, 16, sizeof(RandomType *));

static int cmp_type (const void *pa, const void *pb)
{
	RandomType * const *a = pa;
	RandomType * const *b = pb;
	return mstrcmp(a[0]->name, b[0]->name);
}

void AddRandomType (RandomType *type)
{
	if	(type)
		vb_search(&TypeTab, &type, cmp_type, VB_REPLACE);
}

void ListRandomType (IO *io)
{
	RandomType **type;
	size_t n;

	for (n = TypeTab.used, type = TypeTab.data; n-- > 0; type++)
	{
		io_puts((*type)->name, io);
		io_putc('\t', io);
		io_langputs((*type)->desc, io);
		io_putc('\n', io);
	}
}


RandomType *GetRandomType (const char *name)
{
	size_t n;
	RandomType **type;

	if	(TypeTab.used == 0)
	{
		AddRandomType(&RandomType_std);
		AddRandomType(&RandomType_old);
		AddRandomType(&RandomType_d48);
	}

	for (n = TypeTab.used, type = TypeTab.data; n-- > 0; type++)
		if (mstrcmp(name, (*type)->name) == 0) return *type;

	if	(name)
		log_note(NULL, "[efmain:301]", "s", name);

	return NULL;
}

