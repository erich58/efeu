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

io_t *io_tmpfile (void)
{
	FILE *tmp;

	if	((tmp = tmpfile()) != NULL)
		return io_stream("<tmpfile>", tmp, fclose);

	return io_tmpbuf(TMP_SIZE);
	return NULL;
}


/*	Datei öffnen
*/

io_t *io_fopen (const char *name, const char *mode)
{
	return io_stream(name, fopen(name, mode), fclose);
}

io_t *io_fileopen(const char *name, const char *mode)
{
	if	(name && *name == '|')
		return io_popen(name + 1, mode);

	return io_stream(name, fileopen(name, mode), fileclose);
}

io_t *io_findopen (const char *path, const char *name, const char *ext,
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

static int f_get (FILE_IO *ptr);
static int f_put (int c, FILE_IO *ptr);
static int f_ctrl (FILE_IO *fio, int req, va_list list);

static size_t f_dbread (FILE_IO *fio, void *p, size_t r, size_t s, size_t n)
{
	return dbread(fio->file, p, r, s, n);
}

static size_t f_dbwrite (FILE_IO *fio, void *p, size_t r, size_t s, size_t n)
{
	return dbwrite(fio->file, p, r, s, n);
}


io_t *io_stream (const char *name, FILE *file, int (*fclose) (FILE *))
{
	io_t *io;
	FILE_IO *fio;

	if	(file == NULL)	return NULL;

	fio = memalloc(sizeof(FILE_IO));
	fio->name = mstrcpy(name);
	fio->file = file;
	fio->fclose = fclose;

	io = io_alloc();
	io->get = (io_get_t) f_get;
	io->put = (io_put_t) f_put;
	io->dbread = (io_data_t) f_dbread;
	io->dbwrite = (io_data_t) f_dbwrite;
	io->ctrl = (io_ctrl_t) f_ctrl;
	io->data = fio;
	return io;
}


/*	Lesen eines Zeichens
*/

static int f_get (FILE_IO *ptr)
{
	return getc(ptr->file);
}


/*	Ausgabe eines Zeichens
*/

static int f_put (int c, FILE_IO *ptr)
{
	return putc(c, ptr->file);
}


/*	Kontrollfunktion
*/

static int f_ctrl (FILE_IO *fio, int req, va_list list)
{
	int stat;

	switch (req)
	{
	case IO_CLOSE:

		stat = fio->fclose ? fio->fclose(fio->file) : 0;
		memfree(fio->name);
		memfree(fio);
		return stat;

	case IO_REWIND:
	
		return fseek(fio->file, 0l, SEEK_SET);

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
