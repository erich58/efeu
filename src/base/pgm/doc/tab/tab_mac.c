/*	Befehle abfragen/einfügen
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <efeudoc.h>

void DocMac_clean (DocMac_t *mac)
{
	memfree(mac->name);
	memfree(mac->desc);
	memfree(mac->fmt);
}


void DocMac_print (io_t *io, DocMac_t *mac, int mode)
{
	if	(io && mac && mac->desc)
	{
		reg_fmt(1, "|%s|", mac->name);

		if	(mode & 0x1)
			io_psub(io, "\n\\margin $1\n");

		io_psub(io, mac->desc);

		if	(mode & 0x2)
		{
			io_nlputs("\n---- verbatim", io);
			io_nlputs(mac->fmt, io);
			io_nlputs("----", io);
		}
	}
}

static int mac_cmp (DocMac_t *a, DocMac_t *b)
{
	return mstrcmp(a->name, b->name);
}

DocMac_t *DocTab_getmac (DocTab_t *tab, const char *name)
{
	if	(tab)
	{
		DocMac_t key;
		key.name = (char *) name;
		return vb_search(&tab->mtab, &key, (comp_t) mac_cmp, VB_SEARCH);
	}
	else	return NULL;
}

void DocTab_setmac (DocTab_t *tab, char *name, char *desc, char *fmt)
{
	DocMac_t key;

	key.name = name;
	key.desc = desc;
	key.fmt = fmt;
	vb_search(&tab->mtab, &key, (comp_t) mac_cmp, VB_REPLACE);
	DocMac_clean(&key);
}
