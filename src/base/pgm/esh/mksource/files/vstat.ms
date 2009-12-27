/*
Verarbeitungsstatistik

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

#ifndef	vstat_ms
#define	vstat_ms	1

#include <function.ms>

void VStat(str sname, str label, IO def, str pdef = NULL, bool glob = true)
{
	str name, fmt, val;
	Function f;
	int n;
	IO io = glob ? hdr : src;

	sname = psub(sname);
	label = psub(label);

	if	(glob)
		fprintf(hdr, "\n/*\t%s\n*/\n\n", label);

	fprintf(src, "\n/*\t%s\n*/\n\n", label);
	fprintf(src, "char *%s_fmt[] = {\n", sname);

	for (n = 0; getline(def, name, fmt); n++)
	{
		name = psub(name, sname);

		if	(patcmp("<1->[%n_]", name))
			fprintf(io, "#define %s\t%d\n", name, n);

		fprintf(src, "\t%#s,\n", fmt);
	}

	if	(n == 0)
	{
		fprintf(src, "\t%#s,\t/* %s */\n", NULL, "Dummyeintrag");
		n++;
	}

	src << "};\n";

	fprintf(io, "\n#define %s_dim\t%d\t/* Zahl der Formate */\n\n",
		sname, n);
	fprintf(io, "extern char *%s_fmt[%d];\t/* Formatvektor */\n",
		sname, n);

	if	(!pdef)	return;

	f = Function("void $1_print", "io_t *out, unsigned *tab", sname)
	f.line("int n;");
	f.line();
	f.line("if\t(!out)\treturn;");
	f.line();
	f.line("for (n = 0; n < $1_dim; n++)");
	f.beg();
	fmt = sname + "_fmt[n]";
	val = "tab[n]";
	f.body(pdef);
	f.end();
	f.write(glob);
}

#endif	/* vstat.ms */
