/*	Generierung einer Dezilverteilung
	(c) 1998 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 3.1
*/

#include "dezile.h"

static LimDef_t med_tab[] = {
	{ 0.5, "MED", "Median" }, 
};

static LimDef_t quart_tab[] = {
	{ 0.25, "QW1", "1. Quartil" }, 
	{ 0.50, "QW2", "Median" }, 
	{ 0.75, "QW3", "3. Quartil" }, 
};

static LimDef_t xquart_tab[] = {
	{ 0.10, "DW1", "1. Dezil" }, 
	{ 0.25, "QW1", "1. Quartil" }, 
	{ 0.50, "MED", "Median" }, 
	{ 0.75, "QW3", "3. Quartil" }, 
	{ 0.90, "DW9", "9. Dezil" }, 
};

static LimDef_t dez_tab[] = {
	{ 0.1, "DW1", "1. Dezil" }, 
	{ 0.2, "DW2", "2. Dezil" }, 
	{ 0.3, "DW3", "3. Dezil" }, 
	{ 0.4, "DW4", "4. Dezil" }, 
	{ 0.5, "DW5", "Median" }, 
	{ 0.6, "DW6", "6. Dezil" }, 
	{ 0.7, "DW7", "7. Dezil" }, 
	{ 0.8, "DW8", "8. Dezil" }, 
	{ 0.9, "DW9", "9. Dezil" }, 
};

static ModeDef_t mode_tab[] = {
	{ "median", "Median", med_tab, tabsize(med_tab) },
	{ "quart", "Quartile", quart_tab, tabsize(quart_tab) },
	{ "xquart", "Erweiterte Quartile", xquart_tab, tabsize(xquart_tab) },
	{ "dez", "Dezile", dez_tab, tabsize(dez_tab) },
};

ModeDef_t *GetMode (const char *name)
{
	ModeDef_t *mp;
	int i;

	for (mp = mode_tab, i = tabsize(mode_tab); i-- > 0; mp++)
		if (mstrcmp(name, mp->name) == 0) return mp;

	return NULL;
}

static void print_mode (io_t *io, InfoNode_t *info)
{
	ModeDef_t *mode = info->par;
	LimDef_t *lim = mode->tab;
	size_t n = mode->dim;

	while (n-- > 0)
	{
		io_printf(io, "%3.2f\t%s\t%s\n", lim->x, lim->name, lim->label);
		lim++;
	}
}

void ModeInfo (InfoNode_t *info, const char *name, const char *label)
{
	ModeDef_t *mp;
	int i;

	info = AddInfo(info, name, mstrcpy(label), NULL, NULL);

	for (mp = mode_tab, i = tabsize(mode_tab); i-- > 0; mp++)
		AddInfo(info, mp->name, mstrcpy(mp->label), print_mode, mp);
}
