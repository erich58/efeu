/*
Bitfield - Datenstrukturen generieren

$Header	<$1>
$Copyright (C) 1995 Erich Frühstück
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

#ifndef	bitfield_ms
#define	bitfield_ms	1

#include <macro.ms>
#include <DataType.ms>

static struct BitfieldVar {
	int size;	/* Variablenlänge */
	str name;	/* Variablenname */
	str desc;	/* Beschreibungstext */
};

static BitfieldVar BitfieldVar (int s, str n, str d)
{
	size = s;
	name = n;
	desc = d;
}

static BitfieldVar BitfieldVar (int s, int num)
{
	size = s;
	name = sprintf("_D%d", num);
	desc = "Ausrichtungsdummy";
}

static void BitfieldVar::typeline (IO io)
{
	fprintf(io, "\tunsigned %s : %d;", name, size)

	if	(desc)	fprintf(io, "\t/* %s */", desc);
	
	io << '\n';
}


/*
:*:The function |$1| creates a bitfield data type and accordingly macros
for use in context of EFEU.
:de:Die Funktion |$1| generiert eine Bitfeldstruktur.
Dabei gibt <Name> den Namen der Struktur, <Def> eine Eingabestruktur
mit Variablendefinitionen und <Desc> eine Kurzbezeichnung
der Struktur an.

Eine Definitionszeile in der Eingabestruktur hat die Gestalt:
	Bitlänge Name Beschreibungstext

Die Struktur wird unter dem Namen <Name> generiert.
Zur Ausrichtung auf ganze Wörter (32 Bit) werden bei Bedarf
Dummyvariablen eingeführt.

Neben der Strukturdefinition werden die folgenden Makros definiert:
[<name>|_size|]
	gibt die Bytelänge der Struktur an
[<name>|_recl|]
	gibt die für die binäre Ein/Ausgabe verwendete
	Satzlänge an. Diese kann kleiner als <name>|_recl| sein.
[<name><|_read(<io>, <data>, <dim>)|>]
	liest <dim> Datensätze aus <io> in den Buffer <data>.
[<name><|_write(<io>, <data>, <dim>)|>]
	gibt <dim> Datensätze des Buffers <data> nach <io> aus.
[<name><|_clear(<data>, <dim>)|>]
	setzt den Datenbuffer <buf> mit <dim> Elementen auf 0.

Die Funktion |$1| generiert nur Headerdefinitionen, es werden
keine Sourcezeilen ausgegeben.
*/

void Bitfield(str Name, str Desc, IO Def, bool flag = true)
{
	str name, gname, desc;
	int i, n, unused;
	int size = 0;
	int recl = 0;
	int ndummy = 0;
	Function f;

	Name = psub(Name);
	Desc = Desc ? psub(Desc) : "Bitfeldstruktur " + Name;

/*	Bitfeldeinträge generieren
*/
	DataType type = DataType(Name, NULL);
	DataBase db = DataBase(BitfieldVar);
	str mtab;

	Function x2list = Function("void $1_to_list",
		"EfiFunc *func, void *rval, void **arg", Name);
	x2list.line("$1 *data = arg[0];", Name);
	x2list.line("EfiObjList **ptr = rval;");

	type.dt_print = psub("$1_plain_print", Name);
	Function xprint = Function("int $1_plain_print",
		"const EfiType *type, const void *opaque_data, IO *io", Name);
	xprint.line("const $1 *data = opaque_data;", Name);
	xprint.line("int n = io_puts(\"{\", io);", Name);

	Function list2x = Function("void $1_from_list",
		"EfiFunc *func, void *rval, void **arg", Name);
	list2x.line("$1 *data = rval;", Name);
	list2x.line("EfiObjList *list = Val_list(arg[0]);");

	while (getline(Def, n, name, desc))
	{
		if	(strlen(desc) == 0)	desc = NULL;

		if	(n > 32)	n = 32;

		if	(size + n > 32)
		{
			db += BitfieldVar(32 - size, ++ndummy);
			size = 0; 
			recl += 4;
		}

		db += BitfieldVar(n, name, desc);

		x2list.line("*ptr = NewObjList(uint2Obj(data->$1));", name);
		x2list.line("ptr = &(*ptr)->next;");
		xprint.line("n += io_xprintf(io, \" %uu,\", data->$1);", name);

		list2x.newpar();
		list2x.line("if	(list)");
		list2x.beg();
		list2x.line("data->$1 = Obj2uint(RefObj(list->obj));", name);
		list2x.line("list = list->next;");
		list2x.end();
		list2x.ignorepar();
		list2x.line("else	data->$1 = 0;", name);

		LocalFunction("void $1_$2_get", "unsigned *data, void *ptr",
			"*data = (($1 *) ptr)->$2;", Name, name);
		LocalFunction("void $1_$2_set", "unsigned *data, void *ptr",
			"(($1 *) ptr)->$2 = *data;", Name, name);
		mtab += psub("\t{ $[2;%#s], $[3;%#s], $1_$2_get, $1_$2_set },\n",
			Name, name, desc);
		size += n;

		if	(size == 32)
		{
			recl += 4;
			size = 0;
		}
	}

	x2list.write(false);
	xprint.line("n += io_puts(\" }\", io);");
	xprint.line("return n;");
	xprint.write(false);
	list2x.write(false);

/*	Ungenützte Bytes bestimmen
*/
	if	(size)
	{
		size = 32 - size;
		unused = size / 8;
		size -= 8 * unused;
		recl += 4 - unused;

		if	(size)
			db += BitfieldVar(size, ++ndummy)

		if	(unused)
			db += BitfieldVar(8 * unused, ++ndummy)
	}
	else	unused = 0;


/*	Header ausgeben
*/
	IO io = flag ? hdr : top;
	n = dim(db);

	fpsub(io, "\n/*\t$2\n*/\n", Name, Desc);
	io << "\ntypedef struct {\n";
	io << "#if\tREVBYTEORDER\n";

	for (i = 1; i <= n; i++)
		db[n - i].typeline(io);

	io << "#else\n";

	for (i = 0; i < n; i++)
		db[i].typeline(io);

	io << "#endif\n";

	fpsub(io, "} $1;\n\n", Name);

	if	(mtab)
	{
		fprintf(src, "static EfiBFMember %s_mtab[] = {\n%s};\n\n",
			Name, mtab);
		type.setup("AddEfiBFMember(Type_$1.vtab, \
$1_mtab, tabsize($1_mtab));", Name);
	}

	type.setup("SetFunc(0, &Type_$1, \"$1(List_t)\", $1_from_list);", Name);
	type.setup("SetFunc(0, &Type_list, \"$1()\", $1_to_list);", Name);
	type.dt_recl = sprintf("%d", recl);
	type.write();

/*	Dokumentation ausgeben
*/
	fpsub(info, "@$1:$2\n", Name, Desc);

	for (x in db)
		fprintf(info, "unsigned %s : %d\t%s\n", x.name, x.size, x.desc)

	info << '\n';
}

/*
$Example
Das folgende Beispiel zeigt die Definition einer einfachen
Bitfeldstruktur:

\ms_example
config hdr src

#include <bitfield.ms>

Bitfield("foo", "Test", string !
12	x	Variable 1
8	y	Variable 2
4	z	Variable 3
!);
\end

$SeeAlso
\mref{mksource(1)}, \mref{DataType(7)}.
*/

#endif	/* bitfield.ms */
