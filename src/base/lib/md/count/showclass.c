/*
Auflisten von Selektionsdefinitionen

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

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>


#define	MAX_COLUMNS	64	/* Max. Spaltenzahl */
#define	WIDTH		16	/* Breite für Zusammenfassung */

#define	LBL_CLASS	\
	":*:grouping classes" \
	":de:Selektionsklassen"

char *MdClassListFormat = "$1[$3]\t$2\n";
char *MdClassPrintHead = "$1[$3]\t$2\n";
char *MdClassPrintEntry = "\t$1\t$2\n";
char *MdClassPrintFoot = NULL;
int MdClassPrintLimit = 40;

void MdClassList (IO *io, MdClass *cdef)
{
	io_psubarg(io, MdClassListFormat, "nssd", cdef->name,
		cdef->desc, cdef->dim);
}

static void p_label (IO *io, Label *label)
{
	io_psubarg(io, MdClassPrintEntry, "nss", label->name, label->desc);
}

void MdClassPrint (IO *io, MdClass *cdef)
{
	int i;

	io_psubarg(io, MdClassPrintHead, "nssd", cdef->name,
		cdef->desc, cdef->dim);

	if	(MdClassPrintLimit == 1)
	{
		;
	}
	else if	(MdClassPrintLimit && cdef->dim > max(4, MdClassPrintLimit))
	{
		p_label(io, cdef->label);
		p_label(io, cdef->label + 1);
		io_psubarg(io, MdClassPrintEntry, "nss", "...", "...");
		p_label(io, cdef->label + cdef->dim - 2);
		p_label(io, cdef->label + cdef->dim - 1);
	}
	else
	{
		for (i = 0; i < cdef->dim; i++)
			p_label(io, cdef->label + i);
	}

	io_psubarg(io, MdClassPrintFoot, "nss", cdef->name, cdef->desc);
}

void MdShowClass (IO *io, MdCountTab *tab, const char *plist)
{
	MdClass **ptr;
	size_t n;

	if	(plist)
	{
		char **list;
		size_t dim;

		dim = mstrsplit(plist, ",;%s", &list);

		for (ptr = tab->ctab.data, n = tab->ctab.used; n-- > 0; ptr++)
			if (patselect((*ptr)->name, list, dim))
				MdClassPrint(io, *ptr);

		memfree(list);
	}
	else
	{
		for (ptr = tab->ctab.data, n = tab->ctab.used; n-- > 0; ptr++)
			MdClassList(io, *ptr);
	}
}


static void print_class (IO *io, InfoNode *info)
{
	char *head = MdClassPrintHead;
	char *entry = MdClassPrintEntry;
	MdClassPrintHead = NULL;
	MdClassPrintEntry = "$1\t$2\n";
	MdClassPrint(io, info->par);
	MdClassPrintHead = head;
	MdClassPrintEntry = entry;
}


void MdClassInfo (InfoNode *info, MdCountTab *tab)
{
	InfoNode *root;
	MdClass **ptr;
	size_t n;

	root = AddInfo(info, "class", LBL_CLASS, NULL, NULL);

	for (ptr = tab->ctab.data, n = tab->ctab.used; n-- > 0; ptr++)
		AddInfo(root, (*ptr)->name, (*ptr)->desc, print_class, *ptr);
}
