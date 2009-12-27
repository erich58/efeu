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

static void load (InfoNode *base, IO *io);

static void add_entry(InfoNode *node, StrBuf *sb)
{
	if	(node && node->func == NULL && node->par == NULL)
		node->par = sb_strcpy(sb);
}

static int get_key(IO *io, char **p)
{
	StrBuf *sb;
	int c;

	sb = sb_acquire();

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

	*p = sb_cpyrelease(sb);
	return c;
}

static int get_label(IO *io, char **p)
{
	StrBuf *sb;
	int c;

	sb = sb_acquire();

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

	*p = sb_cpyrelease(sb);
	return c;
}

static char *get_name(IO *io)
{
	StrBuf *sb;
	int c;

	sb = sb_acquire();

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

	return sb_cpyrelease(sb);
}

static void do_load (InfoNode *info)
{
	char *fname = info->par;
	info->par = NULL;

	if	(fname)
	{
		load(info, io_fileopen(fname, "rz"));
		memfree(fname);
	}
}

static InfoNode *get_node(IO *io, InfoNode *node)
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
			node->setup = do_load;
			node->par = fsearch(InfoPath, NULL, p, NULL);
			memfree(p);
		}
	}
	else if	(c == '|')
	{
		if	((p = get_name(io)) != NULL)
		{
			node->setup = do_load;
			node->par = msprintf("|%s", p);
			memfree(p);
		}
	}

	return node;
}

/*	Einträge laden
*/

static void load (InfoNode *base, IO *io)
{
	StrBuf *buf;
	InfoNode *node;
	int last, c;

	if	(io == NULL)	return;

	last = '\n';
	buf = NULL;
	node = (base && !base->func && !base->par) ? base : NULL;
	buf = sb_acquire();

	while ((c = io_getc(io)) != EOF)
	{
		if	(last == '\n' && c == INFO_KEY)
		{
			c = io_getc(io);

			if	(c != INFO_KEY)
			{
				add_entry(node, buf);
				sb_trunc(buf);
				node = NULL;

				if	(c != '\n')
				{
					io_ungetc(c, io);
					node = get_node(io, base);
				}

				last = '\n';
				continue;
			}
		}

		sb_putc(c, buf);
		last = c;
	}

	add_entry(node, buf);
	sb_release(buf);
	io_close(io);
}

void IOLoadInfo (InfoNode *base, IO *io)
{
	load(base, io_refer(io));
}

void LoadInfo (InfoNode *base, const char *fname)
{
	IO *io;

	if	(!fname)	return;

	base = GetInfo(base, NULL);

	if	(*fname == '|')
	{
		io = io_popen(fname + 1, "r");
	}
	else	io = io_findopen(InfoPath, fname, NULL, "rz");

	load(base, io);
}
