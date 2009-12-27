/*
Hilfsfunktionen zur Makrogenerierung

$Copyright (C) 1996 Erich Frühstück
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

#ifndef	_MS_MACRO
#define	_MS_MACRO	1

void Macro(IO io, str name, str args, str def, ...)
{
	name = psub(name, va_list);
	args = strlen(args) ? psub(args) : args;
	def = psub(def);
	fprintf(io, "#define\t%s", name);

	if	(args)
		fprintf(io, "(%s)", args);

	str delim = "\t";

	for (x in split(def, "\n"))
	{
		if	(strlen(x))
		{
			io << delim;
			io << x;
			delim = "\t\\\n\t";
		}
	}

	io << "\n";
}

void Define(IO io, IO def, ...)
{
	fpsub(io, "\n", va_list);

	str name, desc;

	while (getline(def, name, desc))
	{
		io << "#define\t";
		fpsub(io, name);
		io << "\t";
		fpsub(io, xstrcut(desc, "%s"));

		if	(strlen(desc))
		{
			io << "\t/* ";
			fpsub(io, desc);
			io << " */";
		}

		io << '\n';
	}

	io << '\n';
}

void Include(IO io, str def, ...)
{
	fpsub(io, "\n", va_list);

	for (x in split(psub(def, va_list), "%s"))
	{
		if	(strlen(x) == 0)	continue;

		io << "#include ";
		fprintf(io, (x[0] == '<' ? "%s" : "%#s"), x);
		io << '\n';
	}

	io << '\n';
}

void StdInclude (IO io, ...)
{
	for (x in va_list)
		fprintf(io, "#include <%s.h>\n", x);

	io << '\n';
}

#endif	_MS_MACRO
