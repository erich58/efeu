/*
Datenstrukturen generieren

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

#ifndef	struct_ms
#define	struct_ms	1

#include <macro.ms>

/*	Strukturvariablen
*/

struct StructVar {
	str def;	/* Variablendefinition */
	str desc;	/* Beschreibungstext */
};

StructVar StructVar (str def, str desc)
	return { psub(def), psub(desc) }

StructVar StructVar (str type, str name, str desc)
	return { psub(paste(" ", type, name)), psub(desc) }

StructVar StructVar (int size, str name, str desc)
	return { sprintf("unsigned %s : %d", psub(name), size), psub(desc) }

StructVar StructVar (int size, int num)
	StructVar (size, sprintf("_D%d", num), "Ausrichtungsdummy");

void StructVar::typeline (IO io)
{
	fprintf(io, "\t%s;", def)

	if	(desc)	fprintf(io, "\t/* %s */", desc);
	
	io << '\n';
}


/*	Strukturdefinition generieren
*/

void Struct (str Name, str Desc, IO Def, bool flag = true)
{
	int i, n, unused;
	int size = 0;
	int recl = 0;
	int ndummy = 0;

	Name = psub(Name);
	Desc = psub(Desc);
	
	if	(!Desc)	Desc = sprintf("Datenstruktur %s", Name);

	DataBase db = DataBase(StructVar);

	str type, name, desc;

	while (getline(Def, type, name, desc))
	{
		if	(type == "@macro")
			db += StructVar(name, desc);
		else if	(isdigit(type[0]))
			db += StructVar((int) type, name, desc);
		else	db += StructVar(type, name, desc);
	}

	IO io = flag ? hdr : src;
	n = dim(db);

/*	Struktur generieren
*/
	fprintf(io, "\n/*\t%s\n*/\n", Desc);
	io << "\ntypedef struct {\n";

	for (x in db)
		x.typeline(io);

	fprintf(io, "} %s_t;\n\n", Name);

/*	Dokumentation ausgeben
*/
	if	(flag)
	{
		fprintf(info, "@%s:%s", Name, Desc);

		for (x in db)
			fprintf(info, "%s\t%s\n", x.def, x.desc)

		info << '\n';
	}
}

#endif	/* struct.ms */
