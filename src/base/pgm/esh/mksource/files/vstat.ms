/*
Verarbeitungsstatistik

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

#ifndef	vstat_ms
#define	vstat_ms	1

#include <macro.ms>
#include <function.ms>

/*
Die Funktion |$1| konstruiert eine Tabelle mit Formatanweisungen
und optional eine Ausgabefunktion zur Ausgabe einer Verarbeitungsstatistik.

Als Argument benötigt die Funktion |$1| den Namen <sname> und die
Bezeichnung <label> der Verarbeitungsstatistik,
sowie eine IO-Struktur <def> mit Formatdefinitionen.

Aus den einzelnen Formatdefinitionen wird der Vektor
|char *| <sname>|_fmt| konstruiert. Zusätzlich wird 
der Makro <sname>|_dim| mit der Zahl der Einträge im Vektor definiert.

Die einzelnen Formatdefinitionen haben den Aufbau:
	<name>	<fmt>
wobei für <name> eine Parametersubstitution durchgeführt wird und Argument 1
auf den Namen der Verarbeitungsstatistik gesetzt ist.
Falls der resultierende Name ein gültiger C-Bezeichner ist, wird ein
Makro mit der aktuellen Indexnummer in der Formattabelle eingerichtet.
Der Parameter <fmt> wird unverändert als Formatstring in die Tabelle übernommen.

Falls das Argument <pdef> verschieden von |NULL|
ist, wird damit eine Ausgabefunktion mit Prototyp
	|void| <sname>|_func (|[<otype> |*out,|] <vtype> |*tab);|
konstruiert. Der Vektor |tab| besteht aus mindestens <sname>|_dim|
Elementen, die der Reihe nach ausgegeben werden.
Mithilfe von <otype> und <vtype> kann der Typ der
Ausgabestruktur und des Statistikvektors verändert werden.
Falls für <otype> |NULL| übergeben wird, entfällt das Argument |out|.

Die Definition
<pdef> bestimmt den Rumpf der Hauptschleife. Für sie wird eine
Parametersubstitution durchgeführt, wobei Argument 1 auf den Namen der
Verarbeitungsstatistik gesetzt wird.
Als Name für den Laufindex wird <n> verwendet.
Zusätzlich wird vor der Parametersubstitution die Variable
<fmt> auf <sname>|_fmt[n]| und <val> auf |tab[n]| gesetzt.
Das unten angegebene Beispiel zeigt ihre Verwendung.

Falls das Argument <glob> der Funktion |$1| den Wert |true| hat, werden
Formatvektor und Ausgabefunktion mit externer Verknüpfbarkeit definiert.
*/

void VStat (str sname, str label, IO def, str pdef = NULL,
	bool glob = false, str otype = "IO", str vtype = "unsigned")
{
	str name, fmt, val;
	Function f;
	int n;
	IO io = glob ? hdr : top;

	sname = psub(sname);
	label = psub(label, sname);

	fprintf(src, "\n/*\t%s\n*/\n", label);

	if	(glob)
	{
		fprintf(hdr, "\n/*\t%s\n*/\n", label);
	}
	else	fprintf(src, "static ");

	fprintf(src, "char *%s_fmt[] = {\n", sname);

	for (n = 0; getline(def, name, fmt); n++)
	{
		name = psub(name, sname);

		if	(patcmp("<1->[%n_]", name))
			fprintf(io, "#define %s\t%d\n", name, n);

		fprintf(src, "\t%#s,\n", psub(fmt, sname));
	}

	if	(n == 0)
	{
		fprintf(src, "\tNULL,\t/* dummy entry */\n");
		n++;
	}

	src << "};\n";
	fprintf(io, "\n#define %s_dim\t%d\n\n", sname, n);

	if	(glob)
		fprintf(hdr, "extern char *%s_fmt[%d];\n", sname, n);

	if	(!pdef)	return;

	if	(otype)
		otype += " *out, ";

	f = Function("void $1_print", otype + vtype + " *tab", sname);
	f.line("int n;");

	if	(otype)
		f.body("\nif\t(!out)\treturn;");

	f.line();
	f.line("for (n = 0; n < $1_dim; n++)", sname);
	f.beg();
	fmt = sname + "_fmt[n]";
	val = "tab[n]";
	f.body(pdef, sname);
	f.end();
	f.write(glob);
}

/*
Die Funktion |$1| generiert zusätzlich zu einer Ausgabefunktion für
eine |IO|-Struktur eine Protokollfunktion für eine |LogControl|-Struktur.
Ansonsten sind die Parameter gleich wie bei |VStat|.
*/

void LogStat (str sname, str label, IO def, str pdef = NULL,
	bool glob = false, str vtype = "unsigned")
{
	sname = psub(sname);
	VStat(sname, label, def, pdef, glob, "IO", vtype);
	Function f = Function("void $1_log",
		"LogControl *ctrl, " + vtype + " *tab", sname);
	f.line("IO *out = LogOpen(ctrl);");
	f.newpar();
	f.line("if\t(out)");
	f.beg();
	f.line("$1_print(out, tab);", sname);
	f.line("io_close(out);");
	f.end();
	f.write(glob);
}

/*
$Example

Das folgende Beispiel zeigt ein Programm, welches die Zeichen der
Standard&-eingabe unter zuhilfenahme einer Verarbeitungsstatistik
nach verschiedenen Zeichenklassen auszählt.
Anstelle von |IO| soll eine |FILE|-Struktur verwendet werden.
Die angegebene Schablonendatei liefert ein lauffähiges Programm.

\ms_example ctype
config src

#include <vstat.ms>

StdInclude(src, "stdio", "ctype");

VStat("CTYPE", "character type", string !
\$1_SPACE	white space characters
\$1_DIGIT	digits
\$1_OTHER	other characters
!, string !
fprintf(out, "%8u %s\n", $(val), $(fmt));
!, false, "FILE");

GlobalFunction("int main", "int argc, char **argv", string !
unsigned tab[CTYPE_dim];
int c;

memset(tab, 0, sizeof(tab));

while ((c = getchar()) != EOF)
{
	if	(isspace(c))	tab[CTYPE_SPACE]++;
	else if	(isdigit(c))	tab[CTYPE_DIGIT]++;
	else			tab[CTYPE_OTHER]++;
}

CTYPE_print(stderr, tab);
return 0;
!);
\end

$SeeAlso
\mref{mksource(1)}, \mref{macro(7)}, \mref{function(7)}.
*/

#endif	/* vstat.ms */
