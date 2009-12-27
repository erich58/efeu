/*	Befehlstabelle
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
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

REFTYPE(DocTab_reftype, "DocTab", tab_ident, tab_admin);

DocTab_t *DocTab (const char *name)
{
	DocTab_t *x = rd_create(&DocTab_reftype);
	x->name = mstrcpy(name);
	return x;
}
