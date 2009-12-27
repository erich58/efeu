/*
Konstruktion einer Aufzählungsdefinition

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

#ifndef	enum_ms
#define	enum_ms	1

#include <function.ms>

/*
$Description
Eine Aufzählungsdefinition erlaubt die Konvertierung von
Schlüsselwörtern in einen Indexwert.
Die folgenden Funktionen werden definiert:

\ttitem	unsigned <enum>Index (const char *<key>);
	liefert den Index der Aufzählung mit Kennung <key>.
\ttitem	char *<enum>Label (unsigned <index>);
	liefert den Namen der Aufzählung mit Index <index>.
\ttitem	<enum>Dim
	ist ein Makro mit der Zahl der Ausprägungen.

Bei nicht definierten Argumenten (Index außerhalb des zulässigen Bereichs,
unbekannter Schlüssel) wird immer die erste Aufzählungsdefinition verwendet.
*/

static void Enum_macro (IO io, str ename, str name, int n, str desc)
{
	if	(name == /^[[:alnum:]_]+$/)
	{
		fprintf(io, "#define\t%s_%s\t%d", ename, name, n);

		if	(desc)
			fprintf(io, "\t/* %s */", desc);

		io << "\n";
	}
}

/*
Die Funktion |$1| generiert eine Aufzählungsdefinition
mit dem Namen <ename>. Falls Flag <glob> wahr ist,
wird die Auszählungsdefinition global, ansonsten lokal generiert.

Die IO-Struktur <def> enthält Definitionszeilen der Form:
	<key> <desc>

wobei <key> ein Aufzählungsschlüssel und <desc> der zugehörige
Beschreibungstext ist. Die einzelnen Einträge werden beginned bei 0
durchnummeriert.

Falls <key> ein gültiger C-Name ist,
wird eine Makro mit dem Namen <ename>|_|<key>}
und der Laufnummer als Wert konstruiert.

Einem Schlüssel können beliebig viele Kennungen der Form |[|<name>|]|
vorangestellt werden. Für jede dieser Kennungen wird ein Makro mit
der aktuellen Laufnummer generiert. Damit lassen sich Makros für
nichtnumerische Aufzählungsschlüssel definieren.
*/

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
	fprintf(src, "static Label %sTab[] = {\n", ename);

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
		fprintf(src, "\t{ %#s, %#s },\n", name, desc);
	}

	if	(n == 0)
		fprintf(src, "\t{ %#s, %#s },\n", NULL, "Dummyeintrag");

	fprintf(io, "\n#define\t%sDim\t%d\t/* Zahl der Ausprägungen */\n\n",
		ename, n);

	src << "};\n";

	f = Function("unsigned $1Index", "const char *name", ename);
	f.line("register int i;");
	f.line();
	f.line("for (i = 0; i < $1Dim; i++)", ename);
	f.line("\tif (mstrcmp(name, $1Tab[i].name) == 0) return i;", ename);
	f.line();
	f.line("return 0;");
	f.write(glob);

	f = Function("char *$1Label", "unsigned n", ename);
	f.line("return $1Tab[n < $1Dim ? n : 0].name;", ename);
	f.write(glob);
}

/*
$Example
Im nachfolgenden Beispiel wird eine Aufzählungsstruktur für
das Geschlecht generiert.

\ms_example
config hdr src

#include <macro.ms>
#include <enum.ms>

Include(src, HeaderName);

Enum("Geschl", "Geschlecht", string !
-	Unbekannt
M	Männer
W	Frauen
!);
\end
*/

#endif	/* enum.ms */
