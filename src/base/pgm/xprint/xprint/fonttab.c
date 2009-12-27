/*	Sortierung/Abfragen von Formatdefinitionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efio.h>
#include <EFEU/object.h>
#include <EFEU/vecbuf.h>
#include "printpar.h"

static VECBUF(fonttab, 32, sizeof(fontdef_t));

static int cmp_font(fontdef_t *a, fontdef_t *b)
{
	return (b->num - a->num);
}


/*	Suche nach einer Fontdefinition
*/

fontdef_t *getfont(int num)
{
	fontdef_t key, *font;

	key.num = num;
	font = vb_search(&fonttab, &key, (comp_t) cmp_font, VB_SEARCH);
	return font;
}


void loadfont(io_t *io, const char *delim)
{
	int c;
	void *s;
	fontdef_t key;

	while ((c = io_eat(io, "%s")) != EOF)
	{
		if	(listcmp(delim, c))
		{
			io_getc(io);
			break;
		}

		memset(&key, 0, sizeof(fontdef_t));

		if	(io_valscan(io, SCAN_INT, &s))
			key.num = Val_int(s);
		else	key.num = 0;

		io_eat(io, " \t,");
		io_scan(io, SCAN_STR, &s);
		key.init = s;
		io_eat(io, " \t,");
		io_scan(io, SCAN_STR, &s);
		key.exit = s;
		io_mcopy(io, NULL, "\n");
		vb_search(&fonttab, &key, (comp_t) cmp_font, VB_REPLACE);
		memfree(key.init);
		memfree(key.exit);
	}
}
