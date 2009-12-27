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
#include <EFEU/CmdPar.h>

#define FMT_11	"[ftools:11]" \
"$!: closing a file not registered by filenotice().\n"

typedef struct {
	FILE *file;
	char *name;
	char *mode;
	int (*close) (FILE *file);
	int refcount;
} FileTab;

static int file_cmp (const void *pa, const void *pb)
{
	const FileTab *a = pa;
	const FileTab *b = pb;

	if	(a->file < b->file)	return -1;
	else if	(a->file > b->file)	return 1;
	else				return 0;
}

static VECBUF(filetab, 32, sizeof(FileTab));

static void file_debug (const char *type, FileTab *tab)
{
	FILE *log;

	if	(!tab)	return;

	log = LogFile("file", DBG_TRACE);

	if	(!log)	return;

	fprintf(log, "file: %s", type);

	if	(tab->refcount)
		fprintf(log, "[%d]", tab->refcount);

	fputs(" (", log);

	if	(tab->name)
		fprintf(log, "\"%s\"", tab->name);
	else	fprintf(log, "%p", tab->file);

	fprintf(log, ", \"%s\")\n", tab->mode);
}

static void closeall (void)
{
	FileTab *tab;
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
	FileTab tab;

	tab.name = mstrcpy(name);
	tab.mode = mstrcpy(mode);
	tab.file = file;
	tab.close = close;
	tab.refcount = 0;
	file_debug("open", &tab);
	vb_search(&filetab, &tab, file_cmp, VB_REPLACE);
	setup_closeall();
}


int fileclose (FILE *file)
{
	FileTab *tab;
	FileTab key;
	int stat;

	if	(file == NULL || file == stdin ||
		 file == stdout || file == stderr)
		return 0;

	key.file = file;
	tab = vb_search(&filetab, &key, file_cmp, VB_SEARCH);

	if	(tab == NULL)
	{
		dbg_note(NULL, FMT_11, NULL);
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
	vb_search(&filetab, &key, file_cmp, VB_DELETE);
	return stat;
}


FILE *filerefer (FILE *file)
{
	FileTab key, *tab;

	key.file = file;
	tab = vb_search(&filetab, &key, file_cmp, VB_SEARCH);

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
		FileTab key, *tab;

		key.file = file;
		tab = vb_search(&filetab, &key, file_cmp, VB_SEARCH);
		return (tab ? tab->name : "<unknown>");
	}
}


void filedebug (FILE *file, int level, const char *fmt, const char *def, ...)
{
	va_list list;
       	va_start(list, def);
	dbg_vpsub("file", level, fmt, mstrcpy(fileident(file)), def, list);
	va_end(list);
}

void fileerror (FILE *file, const char *fmt, const char *def, ...)
{
	va_list list;
       	va_start(list, def);
	dbg_vpsub("file", DBG_ERR, fmt, mstrcpy(fileident(file)), def, list);
	va_end(list);
	exit(EXIT_FAILURE);
}
