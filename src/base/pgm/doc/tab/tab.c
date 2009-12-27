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


static DocTab_t *tab_admin (DocTab_t *tg, const DocTab_t *src)
{
	if	(tg)
	{
		memfree(tg->name);
		DelVarTab(tg->var);
		vb_clean(&tg->mtab, (clean_t) DocMac_clean);
		vb_free(&tg->mtab);
		memfree(tg);
		return NULL;
	}

	tg = memalloc(sizeof(DocTab_t));
	tg->var = VarTab(VAR_NAME, VAR_BLKSIZE);
	vb_init(&tg->mtab, MACTABSIZE, sizeof(DocMac_t));
	return tg;
}

static char *tab_ident (DocTab_t *tab)
{
	if	(tab->name)
		return msprintf("%s[%d,%d]", tab->name,
			tab->var->tab.dim, tab->mtab.used);

	return msprintf("%p[%d,%d]", tab,
		tab->var->tab.dim, tab->mtab.used);
}

ADMINREFTYPE(DocTab_reftype, "DocTab", tab_ident, tab_admin);

DocTab_t *DocTab (const char *name)
{
	DocTab_t *x = rd_create(&DocTab_reftype);
	x->name = mstrcpy(name);
	return x;
}
