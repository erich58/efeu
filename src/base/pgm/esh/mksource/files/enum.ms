/*
Aufzählungen

$Copyright (C) 1998 Erich Frühstück
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

#ifndef	_Enum_ms
#define	_Enum_ms	1

#include <function.ms>

regex_t Enum_name = "^[[:alnum:]_]+$";

void Enum_macro (IO io, str ename, str name, int n, str desc)
{
	if	(regexec(Enum_name, name))
	{
		fprintf(io, "#define\t%s_%s\t%d", ename, name, n);

		if	(desc)
			fprintf(io, "\t/* %s */", desc);

		io << "\n";
	}
}

void Enum(str ename, str label, IO def, bool glob = true)
{
	str name, desc;
	Function f;
	int n;
	IO io = glob ? hdr : src;

	ename = psub(ename);
	label = psub(label);

	if	(glob)
		fprintf(hdr, "\n/*\t%s\n*/\n\n", label);

	fprintf(src, "\n/*\t%s\n*/\n\n", label);
	fprintf(src, "static char *%sTab[] = {\n", ename);

	for (n = 0; getline(def, name, desc); n++)
	{
		while (name[0] == '[')
		{
			name = substr(name, 1);
			str xname = strcut(name, "]");
			Enum_macro(io, ename, xname, n, desc);
		}

		if	(name[0] == '\\')
			name = substr(name, 1);

		Enum_macro(io, ename, name, n, desc);
		fprintf(src, "\t%#s,\t/* %s */\n", name, desc);
	}

	if	(n == 0)
		fprintf(src, "\t%#s,\t/* %s */\n", NULL, "Dummyeintrag");

	fprintf(io, "\n#define\t%sDim\t%d\t/* Zahl der Ausprägungen */\n\n",
		ename, n);

	src << "};\n";

	f = Function("unsigned $1Index", "const char *name", ename);
	f.line("register int i;");
	f.line();
	f.line("for (i = 0; i < $1Dim; i++)");
	f.line("\tif (mstrcmp(name, $1Tab[i]) == 0) return i;");
	f.line();
	f.line("return 0;");
	f.write(glob);

	f = Function("char *$1Label", "unsigned n", ename);
	f.line("return $1Tab[n < $1Dim ? n : 0];");
	f.write(glob);
}

void ListEnum (str ename, str label, bool glob, ...)
{
	str name, desc;
	Function f;
	int n;
	IO io = glob ? hdr : src;

	if	(glob)
		fprintf(hdr, "\n/*\t%s\n*/\n\n", label);

	fprintf(src, "\n/*\t%s\n*/\n\n", label);
	fprintf(src, "static char *%sTab[] = {\n", ename);
#if	1
	fprintf(src, "\t%#s,", NULL);
	n = 1;
#else
	n = 0;
#endif

	for (name in va_list)
	{
		Enum_macro(io, ename, name, n);

		if	(n % 4 == 0)
			src << "\n";

		fprintf(src, "\t%#s,", name);
		n++;
	}

	src << "\n};\n";

	fprintf(io, "\n#define\t%sDim\t%d\t/* Zahl der Ausprägungen */\n\n",
		ename, n);

	f = Function("unsigned $1Index", "const char *name", ename);
	f.line("register int i;");
	f.line();
	f.line("for (i = 1; i < $1Dim; i++)");
	f.line("\tif (mstrcmp(name, $1Tab[i]) == 0) return i;");
	f.line();
	f.line("return 0;");
	f.write(glob);

	f = Function("char *$1Label", "unsigned n", ename);
	f.line("return $1Tab[n < $1Dim ? n : 0];");
	f.write(glob);
}

#endif	/* _Enum_ms */
