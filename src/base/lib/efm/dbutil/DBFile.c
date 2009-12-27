/*
Arbeiten mit Datenbankdateien

$Copyright (C) 2002 Erich Frühstück
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

#include <EFEU/DBData.h>
#include <EFEU/mstring.h>

static char *ref_ident (const void *data)
{
	const DBFile *file = data;
	StrBuf *sb;
	char *p;

	sb = sb_acquire();
	p = rd_ident(file->io);
	sb_puts(p, sb);
	memfree(p);
	sb_puts(" ", sb);

	switch (file->mode)
	{
	case DBFILE_EBCDIC: sb_puts("ebcdic", sb); break;
	case DBFILE_CONV: sb_puts("conv", sb); break;
	case DBFILE_TEXT: sb_puts("text", sb); break;
	case DBFILE_QTEXT: sb_puts("qtext", sb); break;
	}

	sb_printf(sb, " recl=%d", file->recl);
	sb_printf(sb, " delim=%#s", file->delim);
	sb_puts("", sb);

	return sb_cpyrelease(sb);
}

static void ref_clean (void *data)
{
	DBFile *file = data;
	io_close(file->io);
	lfree(file->data.tab);
	lfree(file->data.buf);
	memfree(file->delim);
	memfree(file);
}

RefType DBFile_reftype = REFTYPE_INIT("DBFile", ref_ident, ref_clean);

DBFile *DBFile_open (IO *io, int mode, size_t recl, const char *delim)
{
	DBFile *file = memalloc(sizeof(DBFile));
	file->io = io;
	file->mode = mode;
	file->recl = recl;
	file->delim = mstrcpy(delim);
	file->save = 0;
	return rd_init(&DBFile_reftype, file);
}


int DBFile_next (DBFile *file)
{
	if	(!file)	return 0;

	if	(file->save)
	{
		file->save = 0;
		return 1;
	}

	switch (file->mode)
	{
	case DBFILE_EBCDIC:

		if	(!DBData_ebcdic(&file->data, file->io, file->recl))
			return 0;

		return 1;

	case DBFILE_CONV:

		if	(!DBData_ebcdic(&file->data, file->io, file->recl))
			return 0;

		DBData_conv(&file->data);
		break;

	case DBFILE_QTEXT:

		if	(!DBData_qtext(&file->data, file->io, NULL))
			return 0;

		return 1;

	default:

		if	(!DBData_text(&file->data, file->io, NULL))
			return 0;

		if	(file->recl)
			DBData_sync(&file->data, file->recl, ' ');

		break;
	}

	if	(file->delim)
		DBData_split(&file->data, file->delim);

	return 1;
}

void DBFile_unread (DBFile *file)
{
	file->save = 1;
}

void DBFile_show (DBFile *file, IO *io)
{
	int i;

	if	(!file && !io && !file->data.recl)	return;

	if	(file->data.dim)
	{
		for (i = 0; i < file->data.dim; i++)
			io_xprintf(io, " %#s", file->data.tab[i]);
	}
	else if	(file->mode == DBFILE_EBCDIC)
	{
		for (i = 0; i < file->data.recl; i++)
			io_putc(ebcdic2ascii(file->data.buf[i]), io);
	}
	else
	{
		for (i = 0; i < file->data.recl; i++)
			io_putc(file->data.buf[i], io);
	}

	io_putc('\n', io);
}
