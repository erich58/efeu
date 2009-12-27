/*
Teilfileausgabe in Bibliothek

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
#include <EFEU/ioctrl.h>
#include <EFEU/mstring.h>
#include <EFEU/parsub.h>
#include <EFEU/MakeDepend.h>
#include <EFEU/procenv.h>

/*	Eingabestruktur
*/

typedef struct {
	io_t *io;	/* Ausgabestruktur */
	char *path;	/* Bibliothek */
} DOPEN;


/*	Zeichen ausgeben
*/

static int dopen_put(int c, DOPEN *dopen)
{
	return io_putc(c, dopen->io);
}


/*	Kontrollfunktion
*/

static int dopen_ctrl(DOPEN *dopen, int req, va_list list)
{
	int stat;
	char *p;

	switch (req)
	{
	case IO_CLOSE:

		stat = io_close(dopen->io);
		memfree(dopen->path);
		memfree(dopen);
		return stat;

	case IO_NEWPART:

		reg_cpy(1, va_arg(list, char *));
		p = parsub(va_arg(list, char *));
		io_puts(p, dopen->io);
		memfree(p);

		p = mstrpaste("/", dopen->path, reg_get(1));
		io_push(dopen->io, io_fileopen(p, "wdz"));
		memfree(p);
		return 0;

	case IO_ENDPART:

		return io_close(io_pop(dopen->io));

	case IO_REWIND:

		return EOF;
		
	default:

		return io_vctrl(dopen->io, req, list);
	}
}

static void create_dir (const char *name)
{
	char *p;

	if	(MakeDepend)
	{
		AddTarget(name);
		return;
	}

	p = mstrpaste(" ", "mkdir -p", name);

	if	(system(p) != 0)
		exit(EXIT_FAILURE);

	memfree(p);
}

/*
Die Funktion |$1| generiert eine Ausgabestruktur, die das
Aufspalten der Ausgabe in mehrere Teildateien erlaubt.
Als Argument wird die Name der Bibliothek <dir> und der
Name des Basisfiles <name> angegeben.

Die Bibliothek <dir> wird bei Bedarf eingerichtet. Die
Basisdatei <name> wird relativ zu <dir> generiert.

$SeeAlso
\mref{io_newpart(3)}.
*/

io_t *diropen (const char *dir, const char *name)
{
	DOPEN *par;
	io_t *io;
	char *p;

	if	(!name)
		return NULL;

	create_dir(dir);

	p = mstrpaste("/", dir, name);
	io = io_fileopen(p, "wdz");
	memfree(p);

	par = memalloc(sizeof(DOPEN));
	par->path = mstrcpy(dir);
	par->io = io;

	io = io_alloc();
	io->put = (io_put_t) dopen_put;
	io->ctrl = (io_ctrl_t) dopen_ctrl;
	io->data = par;
	return io;
}

/*
Die Funktion |$1| generiert eine neue Teilausgabedatei mit Namen <name>.
In die Basisdatei wird stellvertretend <repl> eingesetzt. Dabei
wird Parametersubstitution durchgeführt und Register 1 auf <name> gesetzt.
*/

extern int io_newpart(io_t *io, const char *name, const char *repl)
{
	return io_ctrl(io, IO_NEWPART, name, repl);
}

/*
Die Funktion |$1| schließt eine zuvor mit |io_newpart|
generierte Teilausgabedatei.
*/

extern int io_endpart(io_t *io)
{
	return io_ctrl(io, IO_ENDPART);
}

/*
$Note
Werden |io_newpart| oder |io_endpart| auf eine nicht mit |diropen|
generierte IO-Struktur angewendet, bewirken sie nichts.
*/
