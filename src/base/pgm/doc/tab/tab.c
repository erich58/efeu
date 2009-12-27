/*
Befehlstabelle

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

#define	MACTABSIZE	32
#define	VAR_NAME	"DocVar"
#define	VAR_BLKSIZE	32


static void subclean (void *data)
{
	DocMac_clean(data);
}

static void tab_clean (void *data)
{
	DocTab *doc = data;
	memfree(doc->name);
	DelVarTab(doc->var);
	vb_clean(&doc->mtab, subclean);
	vb_free(&doc->mtab);
	memfree(doc);
}

static char *tab_ident (const void *data)
{
	const DocTab *tab = data;

	if	(tab->name)
		return msprintf("%s[%d,%d]", tab->name,
			tab->var->tab.used, tab->mtab.used);

	return msprintf("%p[%d,%d]", tab,
		tab->var->tab.used, tab->mtab.used);
}

RefType DocTab_reftype = REFTYPE_INIT("DocTab", tab_ident, tab_clean);

DocTab *DocTab_create (const char *name)
{
	DocTab *x = memalloc(sizeof(DocTab));
	x->name = mstrcpy(name);
	x->var = VarTab(VAR_NAME, VAR_BLKSIZE);
	vb_init(&x->mtab, MACTABSIZE, sizeof(DocMac));
	return rd_init(&DocTab_reftype, x);
}
