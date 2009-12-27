/*	Informationseintrag suchen/generieren
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/strbuf.h>
#include <EFEU/patcmp.h>
#include <ctype.h>

static int copy (io_t *io, strbuf_t *sb, int key, const char *delim)
{
	int c, flag;

	flag = 0;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '\\')
		{
			c = io_getc(io);

			if	(c == EOF)
			{
				c = '\\';
			}
			else if	(!(listcmp(delim, c) || isspace(c)))
			{
				switch (c)
				{
				case '\\':	break;
				case 'n':	c = '\n'; break;
				case 't':	c = '\t'; break;
				default:
					if	(c != key)
					{
						io_ungetc(c, io);
						c = '\\';
					}
					break;
				}
			}
		}
		else if	(c == key || listcmp(delim, c))
		{
			break;
		}
		else if	(isspace(c))
		{
			flag = sb->pos ? 1 : 0;
			continue;
		}

		if	(c != 0)
		{
			if	(flag)	sb_putc(' ', sb);

			sb_putc(c, sb);
		}

		flag = 0;
	}

	return c;
}

static InfoNode_t *get_info (InfoNode_t *node, io_t *io,
	const char *delim, int mode)
{
	strbuf_t *sb;
	int c;

	if	(node == NULL)
		node = FindInfoNode(NULL, NULL);

	do
	{
		sb = new_strbuf(0);
		c = copy(io, sb, '.', delim);

		if	(mode)
		{
			node = GetInfoNode(node, sb2str(sb));
		}
		else if	(node)
		{
			sb_putc(0, sb);
			node = FindInfoNode(node, (char *) sb->data);
			del_strbuf(sb);
		}
		else	del_strbuf(sb);
	}
	while	(c == '.');

	io_ungetc(c, io);
	return node;
}

char *GetInfoHead (io_t *io, const char *delim)
{
	strbuf_t *sb = new_strbuf(0);
	io_ungetc(copy(io, sb, EOF, delim), io);
	return sb2str(sb);
}

InfoNode_t *IOGetInfo (InfoNode_t *node, io_t *io, const char *delim)
{
	return get_info(node, io, delim, 1);
}

InfoNode_t *IOFindInfo (InfoNode_t *node, io_t *io, const char *delim)
{
	return get_info(node, io, delim, 0);
}

InfoNode_t *StrGetInfo (InfoNode_t *node, const char *name)
{
	if	(name != NULL)
	{
		io_t *io = io_cstr(name);
		node = get_info(node, io, NULL, 1);
		io_close(io);
		return node;
	}
	else	return InfoNodeRoot(node);
}

InfoNode_t *StrFindInfo (InfoNode_t *node, const char *name)
{
	if	(name != NULL)
	{
		io_t *io = io_cstr(name);
		node = get_info(node, io, NULL, 0);
		io_close(io);
		return node;
	}
	else	return InfoNodeRoot(node);
}
