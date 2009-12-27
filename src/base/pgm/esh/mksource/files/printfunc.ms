/*
:*:creating of output functions
:de:Ausgabefunktionen

$Header	<$1>
$Copyright (C) 1997 Erich Frühstück
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

#ifndef	printfunc_ms
#define	printfunc_ms	1

#include <function.ms>

/*
Die Funktion |$1| dient zur Konstruktion von
Ausgabefunktionen.
Die Eingabestruktur <def> enthält die
einzelnen Definitionszeilen für den Funktionsrumpf.
Das Argument <fname> ist der Name, der für die
Ausgabestruktur eingesetzt wird.
Das Argument <vardef> enthält eine Formatdefinition zur Aufbereitung
des Datenwertes einer Definitionszeile.
Die Variable <in_switch> wird für einen rekursiven
Aufruf von |$1| innerhalb eines Switch-Bereichs
benötigt.

Falls der Rückgabewert verschieden von 0 ist, empfiehlt sich
aus rein optischen Gründen das Einfügen einer Leerzeile in den
Funktionsrumpf.
*/

int Function::pfbody & (IO def, str fname, str vardef, bool in_switch = false)
{
	str key, fmt;
	bool need_break = false;
	bool parflag = false;

	while (getline(def, key, fmt))
	{
		if	(parflag)
		{
			newpar();
			parflag = false;
		}

		if	(key[0] == '@')
		{
			key = substr(key, 1);

			if	(key == "if")
			{
				newpar();
				line("if\t(" + fmt + ")");
				beg();
				pfbody(def, fname, vardef);
				parflag = true;
			}
			else if	(key == "elif")
			{
				end();
				line("else if\t(" + fmt + ")");
				beg();
			}
			else if	(key == "switch")
			{
				line();
				line("switch (" + fmt + ")");
				beg();
				pfbody(def, fname, vardef, true);
				parflag = true;
			}
			else if	(key == "else")
			{
				end();
				line("else");
				beg();
			}
			else if	(key == "case")
			{
				if	(need_break) line("break;");

				this.case(fmt);
				need_break = false;
				continue;
			}
			else if	(key == "default")
			{
				if	(need_break) line("break;");

				this.case(NULL);
				need_break = false;
				continue;
			}
			else if	(key == "end")
			{
				if	(need_break) line("break;");

				in_switch = false;
				end();
				return;
			}

			continue;
		}

		if	(key == ".")	key = NULL;
		else if	(vardef)	key = psub(vardef, key);
		else			key = psub(key);

		if	(fmt == NULL)
		{
			line();
			continue;
		}
		else if	(fmt[0] == '$')
		{
			fline("io_puts(%s, %s);",
				psub(substr(fmt, 1), key), fname);
		}
		else if	(fmt[0] == '"')
		{
			fline("io_puts(%s, %s);", fmt, fname);
		}
		else if	(fmt[0] == '\'')
		{
			fline("io_putc(%s, %s);", fmt, fname);
		}
		else if	(fmt[0] == '@')
		{
			fline("io_puts(%#s, %s);", substr(fmt, 1), fname);
		}
		else if	(fmt[0] == '#')
		{
			fline("io_xprintf(%s, %#s, %s);", fname,
				substr(fmt, 1), key);
		}
		else if	(fmt[0] == '?')
		{
			int n = 1;

			while (fmt[n] == ' ')
				n++;

			fmt = substr(fmt, n);

			if	(fmt[0] == '\'')
				fline("io_putc(%s ? %s, %s);", key, fmt, fname);
			else if	(fmt[0] == '"')
				fline("io_puts(%s ? %s, %s);", key, fmt, fname);
			else
			{
				str v1 = strcut(fmt, ":");
				fline("io_xprintf(%s, %s ? %#s : %#s, %s);",
					fname, key, v1, fmt, key);
			}
		}
		else if	(fmt[0] == '=')
		{
			int n = 1;
			str k = "default";
			line();
			line("switch ($1)", key);
			line("{");

			for (z in split(substr(fmt, 1), ","))
			{
				fline("%s:\tio_puts(%#s, %s); break;",
					k, z, fname);
				k = sprintf("case %2d", n++);
			}

			line("}");
			line();
		}
		else if	(fmt[0] == '\\')
		{
			line(substr(fmt, 1) + ";", key, fname);
		}
		else	line(fmt + ";", key, fname);

		need_break = in_switch;
	}

	return parflag;
}

/*
Die Funktion |$1| erzeugt eine einfache Ausgabefunktionen mit
Rückgabetype und Namen <name>, einer IO-Struktur mit Namen |out| als erstes
Argument und den zusätzlichen Variablen <args>.
*/

void PrintFunc (str name, str args, IO def, ...)
{
	Function f = Function(name, paste(", ", "IO *out", args), va_list);
	f.pfbody(def, "out", NULL);
	f.write();
}

/*
$Description
\shead	Syntax
Die einzelnen Definitionszeilen bestehen aus einem Schlüsselwort <key>
und einem Formatstring <fmt>. Soll das Schlüsselwort Leerzeichen oder
Tabulatoren enthalten, müssen diese mit einem Backslash markiert
werden.
Der Formatstring beginnt beim ersten nichtweißen Zeichen nach
dem Schlüsselwort und kann beliebig viele Leerzeichen oder Tabulatoren
enthalten.
Die Befehlszeilen können beliebig eingerückt werden.

Befehlszeilen, die mit einem Klammeraffen beginnen, dienen
zur Konstruktion von Testblöcken und Switch-Anweisungen.
[|@if|]
	leitet einen Testblock ein. Der Formatstring definiert
	den Testausdruck.
[|@elif|]
	definiert eine Alternative (optional). Der Formatstring definiert
	den Testausdruck.
[|@else|]
	definiert die sonstigen Fälle (optional).
[|@end|]
	beendet einen Testblock oder eine Switch-Anweisung.

[|@switch|]
	leitet ein Switch-Anweisung ein. Der Formatstring definiert
	den Schaltwert.
[|@case|]
	definiert eine spezielle Ausprägung.
[|@default|]
	definiert die sonstigen Fälle in der Switch-Anweisung.

Bei allen anderen Zeilen definiert der Formatstring die Befehlszeile
im Funktionsrumpf und das Schlüsselwort den Datenwert.
Dabei gelten folgende Regeln:
*	Bei einem Punkt als Schlüsselwert wird der Datenwert auf NULL gesetzt.
	(Platzhalter, wenn kein Datenwert benötigt wird)
*	Falls <vardef> verschieden von Null ist, wird
	Parametersubstitution auf <vardef> angewendet, wobei Register 1
	mit dem Schlüsselwort belegt wird.
*	Ansonsten wird Parametersubstitution auf das Schlüsselwort
	angewendet.

Das erste Zeichen im Formatstring gilt als Steuerzeichen und gehört
nicht zum eigentlichen Format.
[|\$|]
	Der Datenwert wird mit Parametersubstitution im Format
	eingebaut und das Resultat ausgegeben.
[|"|]
	Das Format wird als String ausgegeben,
	der Datenwert wird ignoriert.
[|'|]
	Das Format wird als Zeichendefinition ausgegeben,
	der Datenwert wird ignoriert.
[|#|]
	Die Ausgabe erfolgt mit |io_xprintf|
[|?|]
	Der Datenwert liefert einen Testausdruck, das Format muß
	von der Form |"|<str1>|" : "|<str2>|"| , |'|<a>|' : '|<b>|'| oder
	<fmt1>|:|<fmt2> sein. Im letzten Fall kann |:| <fmt2>
	entfallen (wird durch NULL ersetzt) und <fmt1> bzw. <fmt2>
	wird mit |io_xprintf| ausgegeben. Der Datenwert kann eingebaut
	werden! Achtung: Leerzeichen gehören zum jeweiligen Format.
[|=|]
	Das Format besteht aus einer Liste von Strings, der Datenwert bestimmt
	den Index (beginnend bei 0). Der erste String ist Vorgabewert
	für Indexwerte außerhalb des zulässigen Bereichs.

Beginnt das Format mit keinem dieser Zeichen, wird der Datenwert mit
Parametersubstitution eingebaut und das Resultat liefert den Ausdruck
im Funktionsrumpf. Der Name der Ausgabeeinheit steht in Register 2.
Ein Backslash am Anfang der Formatanweisung gilt als Fluchtsymbol.

$SeeAlso
\mref{mksource(1)}, \mref{function(7)}.
*/

#endif	/* printfunc.ms */
