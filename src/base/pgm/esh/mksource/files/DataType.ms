/*
:*:Construct simple datatype
:de:Einfache Datentypkonstruktion

$Header	<$1>
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

#ifndef	DataType_ms
#define	DataType_ms	1

#include <var.ms>
#include <macro.ms>
#include <function.ms>

/*
:*:The function |$1| provides include statements for type definations.
:de:Die Funktion |$1| sorgt für die zur Typedefinition notwendigen
Headerdateien.
*/

void DataTypeHeader (bool glob = true)
{
	static bool need_object = true;
	static bool need_stdtype = true;

	if	(glob && need_object)
	{
		StdInclude(hdr, "EFEU/object");
		need_object = false;
	}

	if	(need_stdtype)
	{
		StdInclude(src, "EFEU/stdtype");
		need_stdtype = false;
	}
}

/*
:*:The structure |$1| allows the construction of a datatype in
context of EFEU.
:de:Die Datenstruktur |$1| dient zur Konstruktion von Datentypen
im Kontext von EFEU.
*/

struct DataType {
	str dt_name;
	str dt_def;
	str dt_base;
	Function dt_setup;
	str dt_recl;
	str dt_print;
};

void DataType::DBMacros (IO out)
{
	fprintf(out, "\n/* compatibility macros, may be removed */\n");
	Macro(out, "$1_recl", NULL,
		dt_recl ? dt_recl : "sizeof($1)", dt_name);
	Macro(out, "$1_size", NULL, "sizeof($1)", dt_name);
	Macro(out, "$1_read", "io, data, dim",
		"\nio_dbread(io, data, $1_recl, $1_size, dim)", dt_name);
	Macro(out, "$1_write", "io, data, dim",
		"\nio_dbwrite(io, data, $1_recl, $1_size, dim)", dt_name);
	Macro(out, "$1_clear", "data, dim",
		"\nmemset(data, 0, (dim) * $1_size)", dt_name);
	fprintf(out, "/* end of compatibility macros */\n");
}

/*
:*:The function |$1| creates macros for use of a simple datatype 
in context of EFEU.
:de:Die Funktion |$1| richtet einen einfachen Datentyp ein, der sowohl
von C-Programmen als auch vom EFEU-Interpreter verwendet werden kann.
Der Datentype <Name> wird über |typedef| von einem
bestehenden Datentype <def> abgeleitet. Satz- und Speicherlänge
wird auf |sizeof(|<Name>|)| gesetzt. Optional kann ein
Basistype <base> angegeben werden.
*/

DataType DataType (str Name, str def = NULL, str base = NULL)
{
	Name = psub(Name);

	if	(def)
		def = psub("typedef " + def + " $1;\n", Name);

	Function setup = Function("void $1_setup", NULL, Name);
	setup.line("static int setup_done = 0;");
	setup.line();
	setup.line("if	(setup_done)	return;");
	setup.line();
	setup.line("setup_done = 1;");
	setup.line("AddType(&Type_$1);", Name);
	return (DataType) { Name, def, base, setup, DataBase(str) };
}

void DataType::setup (str fmt = NULL, ...)
	dt_setup.line(fmt, va_list)

/*
Die Funktion |$1| generiert die Datentypdefinition
*/

void DataType::write (bool glob = true)
{
	if	(!dt_name)	return;

	DataTypeHeader(glob);

	if	(dt_def)
		fprintf(glob ? hdr : top, "%s\n", dt_def);

	if	(glob)
	{
		fpsub(hdr, "extern EfiType Type_$1;\n", dt_name);
	}
	else	src << "static ";

	fpsub(src, "EfiType Type_$1 = COMPLEX_TYPE($1%#s, $1%#s, ", dt_name);
	fpsub(src, "sizeof($1), ", dt_name);

	if	(dt_recl)
	{
		fprintf(src, "%s,\n\t", dt_recl);
	}
	else
	{
		fprintf(src, "sizeof(%s),\n\t", dt_name);
	}

	src << "NULL, NULL, ";
	src << (dt_print ? dt_print : "NULL");
	src << ", ";
	src << (dt_base ? dt_base : "NULL");
	src << ", NULL, NULL, NULL, ";
	src << (dt_recl ? "0" : "TYPE_MALLOC");
	src << ");\n";

	dt_setup.write(glob);
	DBMacros(glob ? hdr : top);
	dt_name = NULL;
}

str DataType ()
{
	return this.dt_name;
}

void DataType ()
{
	this.write(true);
}

/*
$Example
Das folgende Beispiel definiert die Makros für den Datentype "foo",
der von "int" abgeleitet wurde.

\ms_example
config hdr src
#include <DataType.ms>
DataType("foo", "int", "&Type_int");
\end

$SeeAlso
\mref{mksource(1)}, \mref{bitfield(7)}.
*/

#endif	/* DataType.ms */
