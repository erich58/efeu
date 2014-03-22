/*
Informationseintrag suchen

$Copyright (C) 1998 Erich Frühstück
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

#include <EFEU/Info.h>
#include <EFEU/strbuf.h>
#include <EFEU/mstring.h>
#include <ctype.h>

#define	INFO_BSIZE	16


static InfoNode root = STD_INFO(NULL, NULL);

static int cmp_info(const void *pa, const void *pb)
{
	InfoNode * const *a = pa;
	InfoNode * const *b = pb;
	return mstrcmp((*a)->name, (*b)->name);
}

static InfoNode *get_info (InfoNode *base, const char *name, char **nptr)
{
	char *ptr;
	InfoNode ibuf, *info, **ip;

	SetupInfo(base);
	*nptr = NULL;

	if	(name == NULL || *name == 0)
		return base ? base : &root;

	if	(!base || (name && *name == INFO_SEP))
		base = &root;

	ptr = (char *) name;

	for (*nptr = ptr; ptr != NULL; *nptr = ptr)
	{
		ibuf.name = InfoNameToken(&ptr);

		if	(ibuf.name == NULL)
		{
			base = &root;
			continue;
		}

		if	(ibuf.name[0] == '.')
		{
			if	(ibuf.name[1] == 0)
			{
				memfree(ibuf.name);
				continue;
			}

			if	(ibuf.name[1] == '.' && ibuf.name[2] == 0)
			{
				if	(base->prev)	base = base->prev;

				memfree(ibuf.name);
				continue;
			}
		}

		info = &ibuf;
		ip = vb_search(base->list, &info, cmp_info, VB_SEARCH);
		memfree(ibuf.name);

		if	(ip)
		{
			base = *ip;
			SetupInfo(base);
		}
		else	break;
	}

	return base;
}

void SetupInfo (InfoNode *node)
{
	if	(node && node->setup)
	{
		void (*setup) (InfoNode *info) = node->setup;
		node->setup = NULL;
		setup(node);
	}
}

InfoNode *GetInfo (InfoNode *base, const char *name)
{
	char *ptr;
	InfoNode *info;

	info = get_info(base, name, &ptr);

	if	(ptr)
	{
		io_putc(INFO_KEY, ioerr);
		InfoName(ioerr, NULL, info);
		io_xprintf(ioerr, ": Eintrag %s nicht definiert\n", ptr);
		return NULL;
	}

	return rd_refer(info);
}

InfoNode *AddInfo (InfoNode *base, const char *name,
	const char *label, void (*func) (IO *io, InfoNode *info), void *par)
{
	InfoNode *info, **ip;
	char *ptr;

	base = get_info(base, name, &ptr);

	if	(ptr == NULL)
	{
		/*
		fprintf(stderr, "Knoten %s in Verwendung.\n", name);
		*/
		fprintf(stderr, "node %s in use.\n", name);
		return NULL;
	}

	while (ptr != NULL)
	{
		if	(base->list == NULL)
		{
			base->list = memalloc(sizeof(VecBuf));
			vb_init(base->list, INFO_BSIZE, sizeof(InfoNode *));
		}

		info = NewInfo(base, InfoNameToken(&ptr));
		ip = vb_search(base->list, &info, cmp_info, VB_ENTER);
		base = *ip;
	}

	base->label = mlangcpy(label, NULL);
	base->func = func;
	base->par = par;
	return base;
}
