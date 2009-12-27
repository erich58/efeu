/*	Informationsdaten laden
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <ctype.h>

#define	NODE_KEY	'*'

char *InfoPath = NULL;


static void add_entry(InfoNode_t *node, char *str)
{
	InfoEntry_t *entry;

	if	(node)
	{
		entry = vb_next(&node->entry);
		entry->func = TextInfoFunc;
		entry->data = str;
	}
	else	memfree(str);
}

static InfoNode_t *get_node(io_t *io, InfoNode_t *node)
{
	char *name;
	strbuf_t *sb;
	int c;

	node = IOGetInfo(node, io, ":<\n");
	c = io_getc(io);

	if	(c == ':')
	{
		memfree(node->head);
		node->head = GetInfoHead(io, "<\n");
		c = io_getc(io);
	}

	if	(c == '<')
	{
		sb = new_strbuf(0);

		while ((c = io_getc(io)) != EOF)
		{
			if	(c == '\n')	break;
			else if	(isspace(c))	;
			else			sb_putc(c, sb);
		}

		name = sb2str(sb);
		LoadInfo(node, name);
		memfree(name);
		return NULL;
	}

	return node;
}


void LoadInfo (InfoNode_t *prev, const char *fname)
{
	io_t *io;
	strbuf_t *buf;
	InfoNode_t *node;
	int last, c;

/*	Datei öffnen
*/
	if	(fname == NULL)	return;

	io = io_findopen(InfoPath, fname, NULL, "rz");

	if	(io == NULL)	return;

/*	Einträge generieren
*/
	last = '\n';
	buf = NULL;
	node = NULL;

	while ((c = io_getc(io)) != EOF)
	{
		if	(last == '\n' && c == NODE_KEY)
		{
			c = io_getc(io);

			if	(c != NODE_KEY)
			{
				if	(buf)
					add_entry(node, sb2str(buf));

				if	(c != '\n')
				{
					io_ungetc(c, io);
					node = get_node(io, prev);
				}

				buf = node ? new_strbuf(0) : NULL;
				last = '\n';
				continue;
			}
		}

		if	(buf)	sb_putc(c, buf);

		last = c;
	}

	memfree(sb2str(buf));
	io_close(io);
}
