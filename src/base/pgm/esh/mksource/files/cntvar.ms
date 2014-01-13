/*
Zählvariablen generieren

$Header	<$1>
$Copyright (C) 2000 Erich Frühstück
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

#ifndef	cntvar_ms
#define	cntvar_ms	1

#include <function.ms>

/*
Die Funktion |$1| dient zur Konstruktion einer Zählvariable.
Sie wurde |inline| definiert, damit die
lokalen Variablen der rufenden Funktion bei der Parametersubstitution
sichtbar bleiben.
*/

inline void CntVar (str name, str type, str exp, str arg = NULL)
{
	str[] tab = regmatch(/([^:]):(.*)/, type);
	str fmt;

	if	(dim(tab) == 3)
	{
		fmt = sprintf("((%s *) (%%s))[0]", tab[1]);
		fprintf(src, "\n#define\tCT_%s\t%#s\n", name, tab[2]);
	}
	else
	{
		fmt = sprintf("((%s *) (%%s))[0]", type);
		fprintf(src, "\n#define\tCT_%s\t%#s\n", name, type);
	}
	
	Function f_set = Function("int set_" + name,
		"MdCount *cnt, void *buf, const void *data");

	Function f_add = Function("void add_" + name,
		"MdCount *cnt, void *opaque_ptr, void *opaque_buf");
	f_add.line("char *ptr = opaque_ptr;");
	f_add.line("char *buf = opaque_buf;");

	List_t args = { sprintf(fmt, "ptr"), sprintf(fmt, "buf") };

	if	(arg)	f_set.line(arg);

	if	(exp == /=/)
	{
		f_set.body(exp, args);
	}
	else	f_set.line("$2 = " + exp + ";", args);

	f_add.line("$1 = $2;", args);
	f_set.line("return 1;");

	fprintf(src, "#define\tini_%s\tNULL\n", name);

	f_set.write(false);
	f_add.write(false);
}

#endif	/* cntvar.ms */
