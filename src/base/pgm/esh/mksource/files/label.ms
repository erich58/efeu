/*
Label-Tabelle generieren

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

#ifndef	label_ms
#define	label_ms	1

#include <var.ms>

struct LabelEntry {
	str name;
	str desc;
};

str LabelEntry ()
	sprintf("{ %#s, %#s }", this.name, this.desc);

struct Label {
	str name;
	DataBase db;
};

virtual int dim (Label x)	dim(x.db)

Label Label (str name, ...)
	return { psub(name, va_list), DataBase(LabelEntry) };

void Label::add (str name, str desc = NULL)
	db += { name, desc };

void Label::tab (bool glob = false)
{
	int n = dim(db);
	IO io = glob ? hdr : src;

	fpsub(io, "\n#define\t$1_dim\t$2\n", name, n);

	if	(n == 0)
	{
		fpsub(io, "#define\t$1\tNULL\n\n", name);
	}
	else	DeclVec(glob, psub("Label $1_tab", name), n, "$(db[n]:%s)");
}

void Label::macro (bool glob = false)
{
	IO io = glob ? hdr : src;
	int n = 0;

	for (x in db)
	{
		if	(x.name == /^[[:alnum:]_]+$/)
		{
			fprintf(io, "#define\t%s_%s\t%d", name, x.name, n);

			if	(x.desc)
				fprintf(io, "\t/* %s */", x.desc);

			io << "\n";
		}

		n++;
	}
}

void Label::load (IO io)
{
	str a, b; 

	while (getline(io, a, b))
		add(a, b);
}

void Label::range (str nfmt, str lfmt, int first, int last)
	for (int n = first; n <= last; n++)
		add(psub(nfmt), psub(lfmt));

#endif	/* label.ms */
