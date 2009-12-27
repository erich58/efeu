/*
:*:Parameter for EfiInput and EiOutput
:de:IO-Parameter für EfiInput und EfiOutput

$Copyright (C) 2006 Erich Frühstück
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

#ifndef	IOPar_ms
#define	IOPar_ms	1

#include <macro.ms>
#include <function.ms>
#include <struct.ms>

void IOPar (str name, str def, str init = NULL, str clean = NULL,
	bool glob = false)
{
	Function f;

	name = psub(name);

	Struct st = Struct(name, name)
	st.addentry("@macro REFVAR");
	st.addentry(psub(def, name));
	st.type(glob ? hdr : top, name);

	f = Function("void $1_clean", "void *opaque_data", name)
	f.line("$1 *data = opaque_data;", name);
	f.body(clean, name);
	f.line("memfree(data);");
	f.write(false);

	fpsub(src, string !
static RefType $1_reftype = REFTYPE_INIT("$1", NULL, $1_clean);
!, name);

	f = Function("void *$1_create",
		"IO *io, const EfiType *type, void *par,
		const char *opt, const char *arg", name);
	f.line("$1 *data = memalloc(sizeof *data);", name);
	f.body(init, name);
	f.line("return rd_init(&$1_reftype, data);", name);
	f.write(glob);
}

#endif	/* IOPar.ms */
