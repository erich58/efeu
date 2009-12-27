/*	Datenbank öffnen
	(c) 1992 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/


#include <EFEU/efmain.h>
#include "agf.h"
#include "asza81.h"
#include "aszb81.h"
#include "asza91.h"
#include "aszb91.h"
#include "hwz81.h"
#include "hwz91.h"
#include "vz81.h"
#include "pendler81.h"
#include "wohnbau.h"
#include "wbneu.h"


static DBDEF def[] = {
#if	0
	{ "asza81", "Arbeitsstättenzählung 1981: Arbeitsstätte",
		"/Oestz/asza81", ASZA81_DIM, sl_asza81, c_asza81, 's' },
	{ "aszb81", "Arbeitsstättenzählung 1981: Beschäftigte",
		"/Oestz/aszb81", ASZB81_DIM, sl_aszb81, c_aszb81, 'b' },
	{ "asza91", "Arbeitsstättenzählung 1991: Arbeitsstätte",
		"/Oestz/asza91", ASZA91_DIM, sl_asza91, c_asza91, 's' },
	{ "aszb91", "Arbeitsstättenzählung 1991: Beschäftigte",
		"/Oestz/aszb91", ASZB91_DIM, sl_aszb91, c_aszb91, 'l' },
	{ "hwz81", "Häuser und Wohnungszählung 1981",
		"/Oestz/hwz81", HWZ81_DIM, sl_hwz81, c_hwz81, 'x' },
	{ "hwz91", "Häuser und Wohnungszählung 1991",
		"/Oestz/hwz91", HWZ91_DIM, sl_hwz91, c_hwz91, 'x' },
	{ "vz81", "Volkszählung 1981",
		"/Oestz/vz81", VZ81_DIM, sl_vz81, c_vz81, 'p' },
	{ "pendler81", "Volkszählung 1981: Pendler",
		"/Oestz/pendler81", PENDLER81_DIM, sl_pendler81, c_pendler81, 'p' },
	{ "wohnbau", "Wohnbaustatistik",
		"/Oestz/wohnbau", WOHNBAU_DIM, sl_wohnbau, c_wohnbau, 'w' },
	{ "wb94", "Wohnbaustatistik 94",
		"/Oestz/wb94", WOHNBAU_DIM, sl_wohnbau, c_wohnbau, 'w' },
#endif
	{ "wbneu", "Neue Wohnbaustatistik",
		"/Oestz/wbneu", WBNEU_DIM, sl_wbneu, c_wbneu, 'w' },
	{ "wb96", "Neue Wohnbaustatistik 1996",
		"/Oestz/wb96", WBNEU_DIM, sl_wbneu, c_wbneu, 'w' },
};


DBDEF *dbdef(const char *name)
{
	int i;

	for (i = 0; i < tabsize(def); i++)
	{
		if	(strcmp(def[i].name, name) == 0)
			return def + i;
	}

	reg_cpy(1, name);
	liberror(NULL, 1);
	return NULL;
}


void dblist(void)
{
	int i;

	printf("%-7s %-19s %5s %s\n", "Name", "File", "Size", "Beschreibung");

	for (i = 0; i < tabsize(def); i++)
	{
		printf("%-7s %-19s %5d %s\n", def[i].name,
			def[i].file, (int) def[i].size, def[i].desc);
	}
}
