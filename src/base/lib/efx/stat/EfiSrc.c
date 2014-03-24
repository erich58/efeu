/*
:*:code source
:de:Codesource

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
#include <EFEU/mstring.h>

static char *src_ident (const void *data)
{
	const EfiSrc *par = data;
	return mstrcpy(par->cmd);
}

static void src_clean (void *ptr)
{
	memfree(ptr);
}

/*
:de:Die Datenstruktur |$1| definiert den Referenztype für die
Struktur |EfiSrc|.
*/

RefType EfiSrc_reftype = REFTYPE_INIT("EfiSrc", src_ident, src_clean);


/*
:de:Die Funktion |$1| liefert den Pointer auf eine dynamisch generierte und
initialisierte Quellinformation.
*/

EfiSrc *EfiSrc_alloc (const char *fmt, ...)
{
	va_list args;
	EfiSrc *ptr;

	ptr = memalloc(sizeof *ptr);
	va_start(args, fmt);
	ptr->cmd = mvsprintf(fmt, args);
	va_end(args);
	return rd_init(&EfiSrc_reftype, ptr);
}

void EfiSrc_push (EfiStat *efi, EfiSrc *src)
{
	pushstack(&efi->src_stack, efi->src);
	efi->src = src;
}

void EfiSrc_pop (Efi *ptr)
{
	EfiStat *efi = Efi_ptr(ptr);
	rd_deref(efi->src);
	efi->src = popstack(&efi->src_stack, NULL);
}

void EfiSrc_lib (Efi *efi, const char *name, const char *init)
{
	EfiSrc_push(Efi_ptr(efi), EfiSrc_alloc("loadlib(%#s, %#s)",
		name, init));
}

void EfiSrc_hdr (Efi *efi, const char *name)
{
	EfiSrc_push(Efi_ptr(efi), EfiSrc_alloc("include(%#s)", name));
}

void EfiSrc_type (Efi *efi, const char *name)
{
	EfiSrc_push(Efi_ptr(efi), EfiSrc_alloc("extern %s;", name));
}

/*
$SeeAlso
\mref{EfiStat(7)}.
*/

