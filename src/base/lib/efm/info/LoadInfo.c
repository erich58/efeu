/*
Informationsdaten laden

$Copyright (C) 1998 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <EFEU/strbuf.h>
#include <EFEU/parsub.h>
#include <EFEU/ftools.h>
#include <ctype.h>

static void load (InfoNode_t *base, io_t *io);

static void add_entry(InfoNode_t *node, strbuf_t *sb)
{
	if	(sb == NULL)	return;

	if	(node && node->func == NULL && node->par == NULL)
	{
		node->par = sb2str(sb);
	}
	else	del_strbuf(sb);
}

static int get_key(io_t *io, char **p)
{
	strbuf_t *sb;
	int c;

	sb = new_strbuf(0);

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '\\')
		{
			switch (c = io_getc(io))
			{
			case 't':	c = '\t';
			case 'n':	c = '\n';
			case 'f':	c = '\f';
			case 'b':	c = '\b';
			case '\n':
			case '\t':
			case ' ':
			case '<':
			case '|':
			case ':':
			case '\\':	break;
			default:	sb_putc('\\', sb); break;
			}

			sb_putc(c, sb);
		}
		else if	(c == '<' || c == '|' || c == ':' || c == '\n')
		{
			break;
		}
		else if	(c == ' ' || c == '\t' || c == '\f')
		{
			continue;
		}

		sb_putc(c, sb);
	}

	*p = sb2str(sb);
	return c;
}

static int get_label(io_t *io, char **p)
{
	strbuf_t *sb;
	int c;

	sb = new_strbuf(0);

	do	c = io_getc(io);
	while	(c == ' ' || c == '\t');

	for (; c != EOF && c != '\n'; c = io_getc(io))
	{
		if	(c == '\\')
		{
			switch (c = io_getc(io))
			{
			case '\n':
			case '\t':
			case '\b':
			case '\f':
			case ' ':
			case '<':
			case '|':
			case '\\':	break;
			default:	sb_putc('\\', sb); break;
			}

			sb_putc(c, sb);
		}
		else if	(c == '<' || c == '|')
		{
			break;
		}

		sb_putc(c, sb);
	}

	while (sb->pos && isspace(sb->data[sb->pos - 1]))
		sb->pos--;

	*p = sb2str(sb);
	return c;
}

static char *get_name(io_t *io)
{
	strbuf_t *sb;
	int c;

	sb = new_strbuf(0);

	do	c = io_getc(io);
	while	(c == ' ' || c == '\t');

	for (; c != EOF && c != '\n'; c = io_getc(io))
	{
		if	(c == '\\')
		{
			switch (c = io_getc(io))
			{
			case '\n':
			case '\t':
			case '\b':
			case '\f':
			case ' ':
			case '\\':	break;
			default:	sb_putc('\\', sb); break;
			}
		}

		sb_putc(c, sb);
	}

	while (sb->pos && isspace(sb->data[sb->pos - 1]))
		sb->pos--;

	return sb2str(sb);
}

static void do_load (InfoNode_t *info)
{
	char *fname = info->par;
	info->par = NULL;
	info->load = NULL;

	if	(fname)
	{
		load(info, io_fileopen(fname, "rz"));
		memfree(fname);
	}
}

static InfoNode_t *get_node(io_t *io, InfoNode_t *node)
{
	char *p;
	int c;

	c = get_key(io, &p);

	if	(p != NULL)
	{
		node = AddInfo(node, p, NULL, NULL, NULL);
		memfree(p);
	}
/*
	else	node = NULL;
*/

	if	(node == NULL)
	{
		while (c != '\n' && c != EOF)
			c = io_getc(io);

		return NULL;
	}

	if	(c == ':')
	{
		c = get_label(io, &p);

		if	(p)
		{
			memfree(node->label);
			node->label = p;
		}
	}
	
	if	(c == '<')
	{
		if	((p = get_name(io)) != NULL)
		{
			node->load = do_load;
			node->par = fsearch(InfoPath, NULL, p, NULL);
			memfree(p);
		}
	}
	else if	(c == '|')
	{
		if	((p = get_name(io)) != NULL)
		{
			node->load = do_load;
			node->par = msprintf("|%s", p);
			memfree(p);
		}
	}

	return node;
}

/*	Einträge laden
*/

static void load (InfoNode_t *base, io_t *io)
{
	strbuf_t *buf;
	InfoNode_t *node;
	int last, c;

	if	(io == NULL)	return;

	last = '\n';
	buf = NULL;
	node = (base && !base->func && !base->par) ? base : NULL;
	buf = node ? new_strbuf(0) : NULL;

	while ((c = io_getc(io)) != EOF)
	{
		if	(last == '\n' && c == INFO_KEY)
		{
			c = io_getc(io);

			if	(c != INFO_KEY)
			{
				add_entry(node, buf);
				node = NULL;

				if	(c != '\n')
				{
					io_ungetc(c, io);
					node = get_node(io, base);
				}

				buf = node ? new_strbuf(0) : NULL;
				last = '\n';
				continue;
			}
		}

		if	(buf)	sb_putc(c, buf);

		last = c;
	}

	add_entry(node, buf);
	io_close(io);
}

void IOLoadInfo (InfoNode_t *base, io_t *io)
{
	load(base, io_refer(io));
}

void LoadInfo (InfoNode_t *base, const char *fname)
{
	io_t *io;

	if	(!fname)	return;

	base = GetInfo(base, NULL);

	if	(*fname == '|')
	{
		io = io_popen(fname + 1, "r");
	}
	else	io = io_findopen(InfoPath, fname, NULL, "rz");

	load(base, io);
}
