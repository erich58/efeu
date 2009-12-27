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
	IO *io;		/* Ausgabestruktur */
	char *path;	/* Verzeichnis */
} DOPEN;


/*	Zeichen ausgeben
*/

static int dopen_put (int c, IO *io)
{
	DOPEN *dopen = io->data;
	return io_putc(c, dopen->io);
}


/*	Kontrollfunktion
*/

static int dopen_ctrl (IO *io, int req, va_list list)
{
	DOPEN *dopen = io->data;
	int stat;
	char *p, *arg;

	switch (req)
	{
	case IO_CLOSE:

		stat = io_close(dopen->io);
		memfree(dopen->path);
		memfree(dopen);
		return stat;

	case IO_NEWPART:

		arg = va_arg(list, char *);
		p = mpsubarg(va_arg(list, char *), "ns", arg);
		io_puts(p, dopen->io);
		memfree(p);

		p = mstrpaste("/", dopen->path, arg);
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

IO *diropen (const char *dir, const char *name)
{
	DOPEN *par;
	IO *io;
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
	io->put = dopen_put;
	io->ctrl = dopen_ctrl;
	io->data = par;
	return io;
}

/*
Die Funktion |$1| generiert eine neue Teilausgabedatei mit Namen <name>.
In die Basisdatei wird stellvertretend <repl> eingesetzt. Dabei
wird Parametersubstitution durchgeführt und Register 1 auf <name> gesetzt.
*/

int io_newpart(IO *io, const char *name, const char *repl)
{
	return io_ctrl(io, IO_NEWPART, name, repl);
}

/*
Die Funktion |$1| schließt eine zuvor mit |io_newpart|
generierte Teilausgabedatei.
*/

int io_endpart(IO *io)
{
	return io_ctrl(io, IO_ENDPART);
}

/*
$Note
Werden |io_newpart| oder |io_endpart| auf eine nicht mit |diropen|
generierte IO-Struktur angewendet, bewirken sie nichts.
*/
