/*
:*:create simple data types
:de:Einfache Datenstrukturen generieren

$Header	<$1>
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

static struct StructVar {
	str def;	/* declaration */
	str desc;	/* description */
};

static StructVar StructVar (str def, str desc)
	return { psub(def), psub(desc) }

static StructVar StructVar (str type, str name, str desc)
	return { psub(paste(" ", type, name)), psub(desc) }

static StructVar StructVar (int size, str name, str desc)
	return { sprintf("unsigned %s : %d", psub(name), size), psub(desc) }

static StructVar StructVar (int size, int num)
	StructVar (size, sprintf("_D%d", num), "Ausrichtungsdummy");

static void StructVar::typeline (IO io)
{
	fprintf(io, "\t%s;", def)

	if	(desc)	fprintf(io, "\t/* %s */", desc);
	
	io << '\n';
}

/*
:*:The data type |$1| allows the construction of C structures.
:de:Der Datentype |$1| erlaubt die Konstruktion von C Structuren.
*/

struct Struct {
	str s_name;
	DataBase s_db;
}

Struct Struct (str _name, ...)
	return { psub(_name, va_list), DataBase(StructVar) };

/*
:*:The function |$1| adds structure members to the structur
definition.
:de:Die Funktion |$1| erweitert die Strukturdefinitionen entsprechend
der Einträge in <def>. Eine einzelne Zeile hat die Form
	<type> <name> <desc>
wobei <type> der Datentyp, <name> der Variablenname und <desc> der
Beschreibungstext ist. Eine Zahl anstelle von <type> generiert einen
Bitfeldeintrag der entsprechenden Größe. Ein . oder das Schlusselwort
|@macro| anstelle von <type> erzeugt einen Eintrag der Form |"|<name>|;"|.
Vergleiche dazu untenstehendes Beispiel.
*/

void Struct::addentry (IO def)
{
	str type, name, desc;

	while (getline(def, type, name, desc))
	{
		if	(type == "." || type == "@macro")
			s_db += StructVar(name, desc);
		else if	(isdigit(type[0]))
			s_db += StructVar((int) type, name, desc);
		else	s_db += StructVar(type, name, desc);
	}
}
	
/*
:*:The function |$1| writes the structur declaration into the file
<out>. The argument <post> terminates the declaration.
:de:Die Funktuion |$1| schreibt die Strukturdefionition in die Datei
<out>. Das Argument <post> wird der Strukturdefinition nachgestellt.
Dies erlaubt die Ausgabe der Strukturdefinition direkt in einer
|typedef|-Anweisung oder einer Variablendeklaration.
*/

void Struct::write (IO out, str post = ";\n")
{
	out << "struct";

	if	(s_name)
		out << " " << s_name;

	out << " {\n";

	for (x in s_db)
		x.typeline(out);

	out << "}" << post;
}

/*
:*:The function |$1| crates a |typedef| instruction with
the structure declaration.
:de:Die Funktion |$1| generiert eine |typedef|-Anweisung mit
der Strukturdefinition.
*/

void Struct::type (IO out, str name)
{
	out << "\ntypedef ";
	this.write(out, paste(name, " ", ";\n\n"));
}

/*
:*:The function |$1| writes the structure description to the info output.
Die Funktion |$1| schreibt eine Strukturbeschreibung
in die Info-Ausgabedatei.
*/

void Struct::mkinfo (str name, str desc)
{
	fprintf(info, "@%s:%s\n", name, desc);

	for (x in s_db)
		fprintf(info, "%s\t%s\n", x.def, x.desc)

	info << '\n';
}

/*
:*:The function |$1| creates a local structure type (output to source
file).
:de:Die Funktion |$1| generiert einen lokalen Strukturdatentyp
(Ausgabe ins Sourcefile).
*/

void LocalType (str name, str desc, IO def, ...)
{
	name = psub(name, va_list);
	Struct st = Struct((str) NULL);
	st.addentry(psubfilter(def, va_list));

	if	(desc)
		fprintf(src, "\n/*\t%s\n*/\n", psub(desc, name));

	st.type(src, name);
}

/*
:*:The function |$1| creates a global structure type (output to hdr). If <desc>
is not NULL, the structure description is written to the info output.
:de:Die Funktion |$1| generiert einen globalen Strukturdatentyp
(Ausgabe ins Headerfile).
Falls <desc> verschieden von 0 ist, wird eine Strukturbeschreibung
in die Info-Ausgabedatei geschrieben.
*/

void GlobalType (str name, str desc, IO def, ...)
{
	name = psub(name, va_list);
	Struct st = Struct((str) NULL);
	st.addentry(psubfilter(def, va_list));

	if	(desc)
	{
		desc = psub(desc, name);
		fprintf(hdr, "\n/*\t%s\n*/\n", desc);
		st.mkinfo(name, desc);
	}

	st.type(hdr, name);
}

/*
$Example
\ms_example
config hdr info

#include <struct.ms>

StdInclude(hdr, "EFEU/refdata");

GlobalType("Data", "example data type with reference counter", string !
@macro	REFVAR	reference variables (-> refdata(3))
12	b1	bitfield
10	b2	bitfield
int	i1[4]	integer vector
double	d	floting point number
unsigned\ char	c	unsigned character variable
!);
\end
*/

#endif	/* struct.ms */
