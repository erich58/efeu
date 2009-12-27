/*
Formatgesteuerte Erweiterung einer Argumentliste

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

#include <EFEU/ArgList.h>
#include <EFEU/mstring.h>
#include <EFEU/Debug.h>
#include <EFEU/io.h>

/*
Die Funktion |$1| erweitert eine Argumentliste entsprechend der
Anweisungen in <def>.
Dabei handelt es sich um eine Zeichenkette aus
Kennbuchstaben mit der folgenden Bedeutung:
[n]	Null Pointer (kein Argument der Liste)
[p]	Pointer Adresse
[s]	Konstante Zeichenkette
[m]	Dynamische Zeichenkette
[d]	Ganzzahlwert
[u]	Vorzeichenloser Ganzzahlwert
[z]	size_t Ganzzahlwert
[U]	Vorzeichenloser, 64-Bit Ganzzahlwert
[x]	Vorzeichenloser Ganzzahlwert in Hexadezimaldarstellung
[X]	Vorzeichenloser, 64-Bit Ganzzahlwert in Hexadezimaldarstellung
[c]	Zeichen unter Hochkomma
*/

void arg_append (ArgList *args, const char *def, va_list list)
{
	int val;
	unsigned uval;
	uint64_t luval;
	void *ptr;

	if	(def == NULL)	return;

	for (; *def != 0; def++)
	{
		switch (*def)
		{
		case 'n':
			arg_cadd(args, NULL);
			break;
		case 's':
			arg_cadd(args, va_arg(list, char *));
			break;
		case 'm':
			arg_madd(args, va_arg(list, char *));
			break;
		case 'p':
			ptr = va_arg(list, void *);
			arg_madd(args, msprintf("%p", ptr));
			break;
		case 'd':
			val = va_arg(list, int);
			arg_madd(args, msprintf("%d", val));
			break;
		case 'u':
			uval = va_arg(list, unsigned);
			arg_madd(args, msprintf("%u", uval));
			break;
		case 'z':
			arg_madd(args, msprintf("%zu", va_arg(list, size_t)));
			break;
		case 'U':
			luval = va_arg(list, uint64_t);
			arg_madd(args, msprintf("%llu", luval));
			break;
		case 'x':
			uval = va_arg(list, unsigned);
			arg_madd(args, msprintf("%#x", uval));
			break;
		case 'X':
			luval = va_arg(list, uint64_t);
			arg_madd(args, msprintf("%#llx", luval));
			break;
		case 'c':
			val = va_arg(list, int);
			arg_madd(args, msprintf("%#c", val));
			break;
		default:
			dbg_note("ArgList", "[efm:61]"
				"$!: ArgList: undefined format key $1.\n",
				"c", *def);
			break;
		}
	}
}

