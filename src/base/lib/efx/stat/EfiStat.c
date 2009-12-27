/*
:*:administation of interpreter status
:de:Verwalten des Interpreterstatus

$Copyright (C) 2005 Erich Frühstück
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

#include <EFEU/EfiStat.h>
#include <EFEU/strbuf.h>

static void data_info (IO *io, const char *name, void *data, Stack *stack)
{
	char *p;
	int n;

	if	(!data)	return;

	p = rd_ident(data);
	io_printf(io, "\t%s:\t[0] %#s\n", name, p);
	memfree(p);

	for (n = 1; stack; stack = stack->next, n++)
	{
		p = rd_ident(stack->data);
		io_printf(io, "\t\t[%d] %#s\n", n, p);
		memfree(p);
	}
}

static char *stat_ident (const void *data)
{
	const EfiStat *ptr = data;
	StrBuf *sb;
	IO *io;

	sb = sb_create(0);
	io = io_strbuf(sb);
	io_printf(io, "%s {\n", ptr->name); 
	data_info(io, "src", ptr->src, ptr->src_stack);
	io_puts("}\n", io);
	io_close(io);
	return sb2str(sb);
}

static void stat_clean (void *ptr)
{
	memfree(ptr);
}

/*
Die Datenstruktur |$1| definiert den Referenztype für die Struktur |EfiStat|.
*/

RefType EfiStat_reftype = REFTYPE_INIT("EfiStat", stat_ident, stat_clean);


/*
:de:Die Funktion |$1| liefert den Pointer auf eine dynamisch generierte und
initialisierte Kommandoparameterstruktur.
*/

EfiStat *EfiStat_alloc (const char *name)
{
	EfiStat *par = memalloc(sizeof *par);
	par->name = mstrcpy(name);
	return rd_init(&EfiStat_reftype, par);
}

/*
Die Funktion |$1| wird intern zur Bestimmung eines Vorgabewertes
für den Pointer <ptr> auf eine Kommandostruktur verwendet.
Falls für <ptr> ein Nullpointer übergeben wird, liefert die
die Funktion |$1| den Pointer auf eine interne Datenstruktur zurück.
Diese wird bei der ersten Verwendung automatisch initialisiert.
Ansonsten liefert die Funktion <ptr>.
*/

Efi *Efi_ptr (Efi *ptr)
{
	static EfiStat buf = {
		REFDATA(&EfiStat_reftype),
		"default",
	};

	return ptr ? ptr : &buf;
}

/*
$Note
Die hier beschriebenen Funktionen werden von einem Kommando nur
dann benötigt, wenn es mit mehr als einer Interpreterinstanz
arbeitet. Alle anderen Funktionen zur Verwaltung des Interpreters
kännen mit einen Nullpointer anstelle der Statusstruktur
aufgerufen werden, da sie mithilfe von |Efi_ptr| auf
eine interne Variable zurückgreifen.

$SeeAlso
\mref{Resource(3)},
\mref{EfiStat(7)}.
*/

