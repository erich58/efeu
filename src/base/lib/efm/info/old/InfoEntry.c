/*	Eintrag generieren
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <ctype.h>

void TextInfoFunc (io_t *io, void *data)
{
	io_puts(data, io);
}

void InfoEntry (InfoNode_t *node, InfoFunc_t func, void *data)
{
	if	(node)
	{
		InfoEntry_t *entry = vb_next(&node->entry);
		entry->func = func;
		entry->data = data;
	}
}

void TextInfoEntry (InfoNode_t *node, char *str)
{
	InfoEntry(node, TextInfoFunc, str);
}
