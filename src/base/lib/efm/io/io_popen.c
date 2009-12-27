/*
Pipe - IO-Struktur

$Copyright (C) 1994 Erich Frühstück
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


#include <EFEU/io.h>
#include <EFEU/ftools.h>
#include <EFEU/MakeDepend.h>
#include <EFEU/procenv.h>
#include <ctype.h>

#if	QC
#define	NEED_POPEN	1
#else
#define	NEED_POPEN	0
#endif


/*	Ein/Ausgabe auf File
*/

FILE *popen (const char *cmd, const char *type);
int pclose (FILE *stream);

IO *io_popen(const char *proc, const char *mode)
{
	IO *io;
	StrBuf *sb;
	FILE *file;
	int i;

	if	(proc == NULL || mode == NULL)
		return NULL;

	if	(pipe_lock)
		return NULL;

	switch (*mode)
	{
	case 'r':
		file = popen(proc, "r");
		break;
	case 'w':
		if	(MakeDepend && strchr(mode, 'd'))
			return NULL;

		file = popen(proc, "w");
		break;
	default:
		file = NULL;
		break;
	}

	if	(!file)	return NULL;

	sb = new_strbuf(32);
	sb_puts("<!", sb);

	for (i = 0; proc[i] && !isspace(proc[i]); i++)
		sb_putc(proc[i], sb);

	sb_puts(">", sb);
	sb_putc(0, sb);
	io = io_stream((char *) sb->data, file, pclose);
	del_strbuf(sb);
	return io;
}


/*	Implementation von popen()
*/

#if	NEED_POPEN

typedef struct KEY_S {
	struct KEY_S *next;	/* nächster Eintrag */
	FILE *file;		/* File Struktur */
	char *proc;		/* Prozeß */
	char *name;		/* Temporärer Filename */
} KEY;


static KEY *root = NULL;	/* Wurzel für offene Pseudo-Pipes */


/*	Pseudo-Pipe schließen
*/

int pclose(FILE *file)
{
	KEY *entry, **ptr;

	ptr = &root;

	while (*ptr != NULL)
	{
		if	((*ptr)->file == file)
		{
			entry = *ptr;
			*ptr = (*ptr)->next;
			fileclose(file);

			if	(entry->proc)
			{
				system(entry->proc);
				memfree(entry->proc);
			}

			remove(entry->name);
			memfree(entry->name);
			memfree(entry);
			return 0;
		}
		else	ptr = &(*ptr)->next;
	}

	return EOF;
}


/*	Pseudo-Pipe öffnen
*/

FILE *popen(const char *name, const char *mode)
{
	KEY *key, **ptr;
	char *p;

	key = memalloc(sizeof(KEY));
	key->next = NULL;
	key->file = NULL;
	key->proc = NULL;
	key->name = mstrcpy(tmpnam(NULL));

	switch (*mode)
	{
	case 'r':

		p = mstrcat(" > ", name, key->name, NULL);
		system(p);
		memfree(p);
		key->file = fileopen(key->name, "r");
		break;

	case 'w':

		key->file = fileopen(key->name, "w");
		key->proc = mstrcat(" < ", name, key->name, NULL);
		break;

	default:

		break;
	}

	ptr = &root;

	while (*ptr != NULL)
		ptr = &(*ptr)->next;

	*ptr = key;
	return key->file;
}

#endif	/* NEED_POPEN */
