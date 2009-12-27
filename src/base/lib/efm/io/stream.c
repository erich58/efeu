/*
Ein/Ausgabe auf File

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
#include <EFEU/ioctrl.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>

#define	TMP_SIZE	1024	/* Größe für Ersatzbuffer */


/*	Temporäre Datei öffnen: Falls nicht möglich,
	wird ein Temporärer Buffer generiert.
*/

IO *io_tmpfile (void)
{
	FILE *tmp;

	if	((tmp = tmpfile()) != NULL)
		return io_stream("<tmpfile>", tmp, fclose);

	return io_tmpbuf(TMP_SIZE);
	return NULL;
}


/*	Datei öffnen
*/

IO *io_fopen (const char *name, const char *mode)
{
	return io_stream(name, fopen(name, mode), fclose);
}

IO *io_fileopen(const char *name, const char *mode)
{
	if	(name && *name == '|')
		return io_popen(name + 1, mode);

	return io_stream(name, fileopen(name, mode), fileclose);
}

IO *io_findopen (const char *path, const char *name, const char *ext,
	const char *mode)
{
	return io_stream(name, findopen(path, NULL, name, ext, mode),
		fileclose);
}


/*	Stream in IO-Struktur einbinden
*/

typedef struct {
	char *name;
	FILE *file;
	int (*fclose) (FILE *file);
} FILE_IO;

static int f_get (void *ptr);
static int f_put (int c, void *ptr);
static int f_ctrl (void *ptr, int req, va_list list);

static size_t f_dbread (void *par, void *p, size_t r, size_t s, size_t n)
{
	FILE_IO *fio = par;
	return r * dbread(fio->file, p, r, s, n);
}

static size_t f_dbwrite (void *par, const void *p, size_t r, size_t s, size_t n)
{
	FILE_IO *fio = par;
	return r * dbwrite(fio->file, p, r, s, n);
}


IO *io_stream (const char *name, FILE *file, int (*fclose) (FILE *))
{
	IO *io;
	FILE_IO *fio;

	if	(file == NULL)	return NULL;

	fio = memalloc(sizeof(FILE_IO));
	fio->name = mstrcpy(name);
	fio->file = file;
	fio->fclose = fclose;

	io = io_alloc();
	io->get = f_get;
	io->put = f_put;
	io->dbread = f_dbread;
	io->dbwrite = f_dbwrite;
	io->ctrl = f_ctrl;
	io->data = fio;
	return io;
}


/*	Lesen eines Zeichens
*/

static int f_get (void *ptr)
{
	FILE_IO *fio = ptr;
	return getc(fio->file);
}


/*	Ausgabe eines Zeichens
*/

static int f_put (int c, void *ptr)
{
	FILE_IO *fio = ptr;
	return putc(c, fio->file);
}


/*	Kontrollfunktion
*/

static int f_ctrl (void *ptr, int req, va_list list)
{
	FILE_IO *fio = ptr;
	int stat;
	long offset;

	switch (req)
	{
	case IO_CLOSE:

		stat = fio->fclose ? fio->fclose(fio->file) : 0;
		memfree(fio->name);
		memfree(fio);
		return stat;

	case IO_FLUSH:

		return fflush(fio->file);

	case IO_REWIND:
	
		return fseek(fio->file, 0l, SEEK_SET);

	case IO_GETPOS:

		if	((offset = ftell(fio->file)) < 0)
			return EOF;

		*va_arg(list, unsigned *) = offset;
		return 0;

	case IO_SETPOS:

		offset = *va_arg(list, unsigned *);
		return fseek(fio->file, offset, SEEK_SET);

	case IO_STAT:

		return fstat(fileno(fio->file), va_arg(list, struct stat *));

	case IO_PEEK:

		return fpeek(fio->file);

	case IO_IDENT:

		*va_arg(list, char **) = mstrcpy(fio->name ? fio->name :
			fileident(fio->file));
		return 0;

	default:

		return EOF;
	}
}
