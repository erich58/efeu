/*	Dokumentmakros auflisten
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <efeudoc.h>

static char *label(const char *desc)
{
	if	(desc)
	{
		int i;

		for (i = 0; desc[i] != 0; i++)
			if (desc[i] == '\n') break;

		return mstrncpy(desc, i);
	}
	else	return NULL;
}

static void mac_add (InfoNode_t *info, DocMac_t *mac)
{
	AddInfo(info, mac->name, label(mac->desc), NULL,
		msprintf("%s\n---- verbatim\n%s//END\n----\n",
			mac->desc, mac->fmt));
}

static void mac_load (InfoNode_t *info)
{
	info->load = NULL;
	info->par = NULL;

	if	(GlobalDocTab)
	{
		size_t n = GlobalDocTab->mtab.used;
		DocMac_t *mp = GlobalDocTab->mtab.data;

		while (n-- > 0)
			mac_add(info, mp++);
	}
}

void DocMacInfo (const char *name, const char *desc)
{
	InfoNode_t *info = AddInfo(NULL, name, mstrcpy(desc), NULL, NULL);
	info->load = mac_load;
}
