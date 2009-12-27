/*
:*:     macro definations
:de:    Makrodefinitionen

$Copyright (C) 2008 Erich Frühstück
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

#include <src2doc.h>
#include <EFEU/object.h>
#include <EFEU/cmdsetup.h>
#include <EFEU/MatchPar.h>
#include <EFEU/Op.h>

typedef struct {
	int i_name;
	int i_arg;
	int i_def;
	int i_desc;
} ENTRY;

static SB_DECL(buf_name, 0);
static VECBUF(buf_entry, 32, sizeof(ENTRY));

void MacDef_clean (void)
{
	sb_trunc(&buf_name);
	buf_entry.used = 0;
}

void MacDef_add (IO *ein)
{
	ENTRY *entry;
	StrBuf *sb, *tmp;
	char *p;
	int c;

	entry = vb_next(&buf_entry);
	sb = &buf_name;
	c = skip_blank(ein);
	p = parse_name(ein, c);

	entry->i_name = sb_getpos(sb);
	sb_puts(p, sb);
	sb_putc(0, sb);

	entry->i_arg = sb_getpos(sb);

	if	(io_peek(ein) == '(')
	{
		IO *aus = io_strbuf(sb);
		sb_putc(io_getc(ein), sb);
		copy_block(ein, aus, ')', 1);
		io_close(aus);
	}

	sb_putc(0, sb);

	tmp = parse_open();
	entry->i_def = sb_getpos(sb);

	while ((c = io_skipcom(ein, tmp, 0)) != EOF)
	{
		if	(c == '\n')	break;
		if	(c == '\\')	c = io_getc(ein);

		sb_putc(c, sb);
	}

	sb_putc(0, sb);

	p = parse_close(tmp);
	entry->i_desc = sb_getpos(sb);
	sb_puts(p, sb);
	sb_putc(0, sb);
}

char *MacDef_name(void)
{
	return buf_entry.used ? (char *) buf_name.data : NULL;
}

void MacDef_source (StrBuf *sb)
{
	ENTRY *entry = buf_entry.data;
	char *p = (char *) buf_name.data;
	int n;

	for (n = buf_entry.used; n--; entry++)
	{
		sb_printf(sb, "#define %s%s%s/* %s*/\n",
			p + entry->i_name, p + entry->i_arg,
			p + entry->i_def, p + entry->i_desc);
	}
}

void MacDef_synopsis (StrBuf *sb)
{
	ENTRY *entry = buf_entry.data;
	char *p = (char *) buf_name.data;
	int n;

	for (n = buf_entry.used; n--; entry++)
	{
		sb_puts("\\index{", sb);
		sb_puts(p + entry->i_name, sb);
		sb_puts("}", sb);
		sb_puts(p + entry->i_arg, sb);
		sb_puts("\\br\n", sb);
	}

	sb_puts("\n", sb);
}

void MacDef_list (StrBuf *sb)
{
	ENTRY *entry = buf_entry.data;
	char *p = (char *) buf_name.data;
	int flag = 0;
	int n;

	for (n = buf_entry.used; n--; entry++)
	{
		char *desc = p + entry->i_desc;

		if	(*desc == 0)	continue;

		sb_puts("\\ttitem ", sb);
		sb_puts(p + entry->i_name, sb);
		sb_puts(p + entry->i_arg, sb);
		sb_puts("\n", sb);
		sb_puts(desc, sb);
		sb_puts("\n", sb);
		flag = 1;
	}

	if	(flag)
		sb_puts("\\endlist\n", sb);
}

