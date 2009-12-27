/*
Konstruktion von Makrodefinitionen

$Header	<$1>
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

#ifndef	macro_ms
#define	macro_ms	1

/*
Die Funktion |$1| dient zur Konstruktion einer Makrofunktion.
Als Argumente wird der Makroname <name>, die Argumentliste
<args> und der Makrorumpf <def> übergeben.
Die Makrodefinition wird in die Datei <io> geschrieben. Hier wird
wahlweise |hdr| oder |src| eingesetzt.

Wird als Argumentliste ein Nullpointer übergeben, wird ein Makro
ohne Argument generiert. Bei einem Leerstring wird ein
Makro mit 0 Argumenten generiert.

Die zusätzlichen Argumente werden als der Reihe nach in den
Stringregistern beginnend bei 1 gespeichert und können damit
über Parametersubstitution, die auf die drei anderen Argumente
angewendet wird, eingebaut werden.

Bei Zeilenvorschüben im Makrorumpf werden automatisch entsprechende
Fortsetzungszeilen in der Makrodefinition generiert.
*/

void Macro (IO io, str name, str args, str def, ...)
{
	name = psub(name, va_list);
	args = strlen(args) ? psub(args, va_list) : args;
	def = psub(def, va_list);
	fprintf(io, "#define\t%s", name);

	if	(args)
		fprintf(io, "(%s)", args);

	str delim = "\t";

	for (x in split(def, "\n"))
	{
		if	(strlen(x))
			io << delim << x;

		delim = "\t\\\n\t";
	}

	io << "\n";
}

/*
Die Funktion |$1| erlaubt die Konstruktion einfacher Makrodefinitionen.
Jede Zeile der Eingabestruktur <def> besteht aus dem
Makronamen, dem Ersetzungstext und einem Beschreibungstext.
Besteht der Ersetzungstext aus mehr als einem Word, muß er unter
Anführung gestellt werden.
*/

void Define (IO io, IO def, ...)
{
	io << "\n";

	str name, desc;

	while (getline(def, name, desc))
	{
		io << "#define\t";
		fpsub(io, name, va_list);
		io << "\t";
		fpsub(io, xstrcut(desc, "%s"), va_list);

		if	(strlen(desc))
		{
			io << "\t/* ";
			fpsub(io, desc, va_list);
			io << " */";
		}

		io << '\n';
	}

	io << '\n';
}

/*
Die Funktion |$1| generiert für jedes übergebene Argument
ein |#include|-Statement. Enthält das Argument keine spitzen Klammern,
wird es unter Anführung gestellt.
*/

void Include (IO io, ...)
{
	for (x in va_list)
	{
		if	(strlen(x))
		{
			io << "#include ";
			fprintf(io, (x[0] == '<' ? "%s" : "%#s"), x);
			io << '\n';
		}
	}

	io << '\n';
}

/*
Die Funktion |$1| generiert für jedes übergebene Argument
ein |#include|-Statement der Form |#include <|<arg>|.h>|.
*/

void StdInclude (IO io, ...)
{
	for (x in va_list)
		fprintf(io, "#include <%s.h>\n", x);

	io << '\n';
}

/*
$Example
Das nachfolgende Beispiel zeigt die Anwendung der obigen Funktionen.

\ms_example
config hdr

#include <macro.ms>

Include(hdr, "<EFEU/config.h>", "local.h");
StdInclude(hdr, "EFEU/mstring", "ctype");

Define(hdr, string !
FLAG_A	0x1	Flag A
FLAG_B	0x2	Flag B
FLAG_C	0x4	Flag C
FLAG_X	"FLAG_B | FLAG_C"	Flag B oder C
!);

Macro(hdr, "Einfach", NULL, "1.5");
Macro(hdr, "Test\$1", "x,y", "((x) + \$1 * (y))", 5);
Macro(hdr, "Print_\$1", "", string !
printf(
"\$1: %s\\n", 
\$1)
!, "Var");
\end

*/

#endif	/* macro.ms */
