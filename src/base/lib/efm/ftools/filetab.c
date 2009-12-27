/*	Hilfsprogramme zum öffnen von Files
:*:	administration of FILE-pointers
:de:	Administration von FILE-Pointern

$Copyright (C) 1994, 2001 Erich Frühstück
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

#include <EFEU/ftools.h>
#include <EFEU/vecbuf.h>
#include <EFEU/patcmp.h>
#include <EFEU/procenv.h>
#include <EFEU/Debug.h>


typedef struct {
	FILE *file;
	char *name;
	char *mode;
	int (*close) (FILE *file);
	int refcount;
} FileTab_t;

static int file_cmp (const FileTab_t *a, const FileTab_t *b)
{
	if	(a->file < b->file)	return -1;
	else if	(a->file > b->file)	return 1;
	else				return 0;
}

static VECBUF(filetab, 32, sizeof(FileTab_t));

static void file_debug (const char *type, FileTab_t *tab)
{
	io_t *io;

	if	(!tab)	return;

	io = LogOut("file", DBG_TRACE);

	if	(!io)	return;

	io_printf(io, "file: %s", type);

	if	(tab->refcount)
		io_printf(io, "[%d]", tab->refcount);

	io_puts(" (", io);

	if	(tab->name)
		io_printf(io, "%#s", tab->name);
	else	io_printf(io, "%p", tab->file);

	io_printf(io, ", %#s)\n", tab->mode);
}

static void closeall (void)
{
	FileTab_t *tab;
	int i;

	tab = filetab.data;

	for (i = 0; i < filetab.used; i++)
	{
		file_debug("forced close", tab + i);

		if	(tab[i].close)
			tab[i].close(tab[i].file);

		memfree(tab[i].name);
	}

	filetab.used = 0;
}

static void setup_closeall (void)
{
	static int closeall_registered = 0;

	if	(closeall_registered)	return;

	atexit(closeall);
	closeall_registered = 1;
}

void filenotice (const char *name, const char *mode,
	FILE *file, int (*close) (FILE *file))
{
	FileTab_t tab;

	tab.name = mstrcpy(name);
	tab.mode = mstrcpy(mode);
	tab.file = file;
	tab.close = close;
	tab.refcount = 0;
	file_debug("open", &tab);
	vb_search(&filetab, &tab, (comp_t) file_cmp, VB_REPLACE);
	setup_closeall();
}


int fileclose (FILE *file)
{
	FileTab_t *tab;
	FileTab_t key;
	int stat;

	if	(file == NULL || file == stdin ||
		 file == stdout || file == stderr)
		return 0;

	key.file = file;
	tab = vb_search(&filetab, &key, (comp_t) file_cmp, VB_SEARCH);

	if	(tab == NULL)
	{
		message("fileclose", MSG_FTOOLS, 11, 0);
		return EOF;
	}

	if	(tab->refcount)
	{
		file_debug("deref", tab);
		tab->refcount--;
		return 0;
	}

	file_debug("close", tab);
	stat = (tab->close ? tab->close(tab->file) : 0);
	memfree(tab->name);
	vb_search(&filetab, &key, (comp_t) file_cmp, VB_DELETE);
	return stat;
}


FILE *filerefer (FILE *file)
{
	FileTab_t key, *tab;

	key.file = file;
	tab = vb_search(&filetab, &key, (comp_t) file_cmp, VB_SEARCH);

	if	(tab)
	{
		tab->refcount++;
		file_debug("refer", tab);
	}

	return file;
}


char *fileident (FILE *file)
{
	setup_closeall();

	if	(file == NULL)		return NULL;
	else if	(file == stdin)		return "<stdin>";
	else if	(file == stdout)	return "<stdout>";
	else if	(file == stderr)	return "<stderr>";
	else
	{
		FileTab_t key, *tab;

		key.file = file;
		tab = vb_search(&filetab, &key, (comp_t) file_cmp, VB_SEARCH);
		return (tab ? tab->name : "<unknown>");
	}
}


void filemessage (FILE *file, const char *name, int num, int narg, ...)
{
	va_list list;
       	va_start(list, narg);
	vmessage(fileident(file), name, num, narg, list);
	va_end(list);
}

void fileerror (FILE *file, const char *name, int num, int narg, ...)
{
	va_list list;
       	va_start(list, narg);
	vmessage(fileident(file), name, num, narg, list);
	va_end(list);
	exit(EXIT_FAILURE);
}
