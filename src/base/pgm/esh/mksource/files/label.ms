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

/*
Der Datentyp |$1| definiert einen einzelnen Eintrag in einer
Label-Tabelle.
*/

struct LabelEntry {
	str name;
	str desc;
};

/*
Der Datentyp |$1| dient zur Generierung von Label-Tabellen.
Diese werden primär bei der Konstruktion von Zählklassen verwendet.
*/

struct Label {
	str name;
	LabelEntry[] entry;
};

Label Label (str name, ...)
	return { psub(name, va_list), NULL };

/*
Die Funktion |$1| liefert die akzuelle Zahl der Label-Einträge
*/

virtual int dim (Label & x)
{
	return dim(x.entry);
}

/*
Die Funktion |$1| ergänzt die Label-Tabelle um einen Eintrag.
*/

void Label::add (str name, str desc = NULL)
	entry.append((name, desc))

/*
Die Funktion |$1| ladet Label-Definitionen aus einer IO-Struktur
*/

void Label::load (IO io)
{
	str a, b; 

	while (getline(io, a, b))
		entry.append((a, b));
}

/*
Die Funktion |$1| generiert Label-Definitionen für einen
vordefinierten Wertebereich.
*/

void Label::range (str nfmt, str lfmt, int first, int last)
	for (int n = first; n <= last; n++)
		entry.append((psub(nfmt), psub(lfmt)));

/*
Die Funktion generiert eine Label-Tabelle vom Typ |Label|.
*/

void Label::tab (bool glob = false)
{
	int n = entry.dim;
	IO io = glob ? hdr : src;

	fprintf(io, "\n#define\t%s_dim\t%d\n\n", name, n);

	if	(n == 0)
	{
		fpsub(io, "#define\t$1_tab\tNULL\n\n", name);
		return;
	}

	if	(glob)
		fprintf(hdr, "extern Label %s_tab[%d];\n", name, n);
	else	src << "static ";
	
	fprintf(src, "Label %s_tab[%d] = {\n", name, n);

	for (x in entry)
		fprintf(src, "\t{ %#s, %#s },\n", x.name, x.desc);

	src << "};\n\n";

}

/*
Die Funktion |$1| generiert eine Liste von Makros für die einzelnen
Einträge der Tabelle.
*/

void Label::macro (bool glob = false)
{
	IO io = glob ? hdr : src;
	int n = 0;

	for (x in entry)
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

/*
$Description

Die Konstruktionsfunktionen |Label::add|, |Label::load| und
|Label::range| können beliebig miteinander kombiniert werden.
Es können sowohl Makrodefinitionen mit |Label::macro| als auch eine
Label-Tabelle mit |Label::tab| generiert werden.
*/

/*
$Examples

Das folgende Beispiel zeigt die Verwendung von |Label|.

\ms_example label
config hdr src

#include <label.ms>

Label lbl = Label("LOAD");
lbl.load(string !
A	Label A
B	Label B
!);
lbl.macro(true);

lbl = Label("RANGE");
lbl.range("R$(n)", "Label R$(n)", 1, 5);
lbl.tab(true);

\end

$SeeAlso
\mref{mksource(1)}, \mref{cntdef(7)}, \mref{index(7)}}.
*/

#endif	/* label.ms */
