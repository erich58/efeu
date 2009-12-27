/*
Konstruktion von Funktionen

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

#ifndef	function_ms
#define	function_ms	1

#include <macro.ms>

/*
Der Datentyp |$1| definert eine Zeile in der konstruierten Funktion.
Dabei ist <margin> die zugehörige Einrücktiefe und <line> der
Zeilentext.
*/

struct FunctionLine {
	int margin;	/* Rand */
	str line;	/* Zeilentext */
};

/*
Die Funktion |$1| schreibt eine Funktionszeile in das Sourcefile.
*/

void FunctionLine::write ()
{
	if	(strlen(line))
		src << "\t" * margin << line;

	src << '\n';
}

/*
Der Datentyp |$1| dient zur Generierung einer C-Funktion.
Einige Komponentennamen beginnen mit |_| um Namenskonflikte bei
der Parametersubstitution zu vermeiden.
Alle Komponentenfunktionen mit Parametersubstitution sind inline,
damit auf lokale Variablen zugegriffen werden kann.
*/

struct Function {
	str _name;
	str _arg;
	int depth;
	bool need_par;
	bool allow_par;
	int _nb;
	int _nh;
	FunctionLine[] _body;
	FunctionLine[] _head;
};

/*
Der Konverter auf |bool| testet, ob die Struktur eine
Funktionsdefinition enthält.
*/

restricted bool Function ()
	return (bool) _name

/*
Der Konverter auf |str| liefert den Prototype der Funktion.
*/

str Function ()	
	return _name ? sprintf("%s(%s)", _name, _arg ? _arg : "void") : NULL;

/*
Beim Konstruktor |$1| gibt die Variable <_name> den Datentyp und Namen der
Funktion an. Das Argument <_args> bestimmt die Argumentliste der Funktion.
Auf <_name> und <_args> wird eine Parametersubstitution angewendet.
Alle weiteren Argumente setzen Stringregister,
die bei der Parametersubstitution zur Verfügung stehen.
*/

Function Function (str _name)
	return { psub(_name), NULL, 1, false, false };

inline Function Function (str _name, str _args, ...)
{
	_name = psub(_name, va_list);
	_args = psub(_args, va_list);
	return { _name, _args, 1, false, false };
}

/*
Mit Hilfe der Funktion |$1| wird die C-Funktion
ausgegeben. Das Argument <global> bestimmt, ob die Funktionsdefinition
global oder statisch erfolgen soll. Bei einer globalen Definition wird
auch ein Prototyp in die Headerdatei geschrieben.
*/

void Function::write (bool global = false)
{
	if	(!_name)	return;

	str proto = sprintf("%s (%s)", _name, _arg ? _arg : "void");

	if	(global)
	{
		hdr << "extern " << proto << ";\n";
	}
	else
	{
		proto = "static " + proto;
		top << proto << ";\n";
	}

	src << "\n";
	src << proto;
	src << "\n{\n";

	for (x in _head)
		x.write();

	if	(dim(_head))
		src << "\n";

	for (x in _body)
		x.write();

	if	(!dim(_head) && !dim(_body))
		src << ";";

	src << "}\n\n";
}

/*
Die folgenden Hilfsfunktionen dienen zum Einfügen und Gliederen
der generierten Sourcezeilen:

\ttitem	$1
	wird zu Beginn jeder Zeile aufgerufen und fügt bei Bedarf eine
	Leerzeile ein.
*/

void Function::startline & (void)
{
	if	(need_par)
	{
		_body[_nb++] += { 0, NULL };
		need_par = false;
	}

	allow_par = true;
}

/*
\ttitem	$1
	fügt eine Absatztrennung ein.
*/

void Function::newpar & (void)
	need_par = allow_par;

/*
\ttitem	$1
	ignoriert nachfolgende Absatztrennungen.
*/

void Function::ignorepar & (void)
	need_par = allow_par = false;

/*
\ttitem	$1
	Erweitert die Liste der Zeilen um <cmd>. Bei einer Leerzeile
	wird |newpar| aufgerufen.
*/

void Function::add_line & (int dp, str cmd)
{
	if	(strlen(cmd))
	{
		startline();
		_body[_nb++] = { dp, cmd };
	}
	else	newpar();
}

/*
\shead	Konstruktion von Sourcezeilen
Zur Konstruktion von Sourcezeilen stehen eine Reihe von Funktionen
zur Verfügung. Die Sourcezeilen werden mit einem Formatargument <fmt>
übergeben, das mit Paramatersubstitution überarbeitet wird.

\ttitem	$1
	fügt eine Befehlszeile ein.
*/

void Function::line & (str _fmt = NULL, ...)
	add_line(depth, psub(_fmt, va_list));

/*
\ttitem	$1
	fügt eine Befehlszeile ein. Der Formatstring <fmt> wird
	jedoch nicht mit Parametersubstitution, sondern mit |sprintf|
	formatiert. Die Stringregister werden nicht verändert.
*/

void Function::fline & (str fmt, ...)
	add_line(depth, sprintf(fmt, va_list));

/*
\ttitem	$1
	fügt eine Zeile ohne Einrückung ein und wird vorzugsweise für
	Preprozessorzeilen verwendet.
*/

void Function::ppline & (str _fmt = NULL, ...)
	add_line(0, psub(_fmt, va_list));

/*
\ttitem	$1
	fügt einen Kommentar ein.
*/

inline void Function::comment & (str _fmt = NULL, ...)
{
	startline();
	_body[_nb++] += { 0, "/*\t" + psub(_fmt, va_list) };
	_body[_nb++] += { 0, "*/" };
	newpar();
}

/*
\ttitem	$1
	zerlegt den Formatstring <_fmt> in einzelne Zeilen
	und fügt sie in der Datenstruktur ein.
*/

inline void Function::body & (str _fmt = NULL, ...)
	for (x in split(psub(_fmt, va_list), "\n"))
		add_line(depth, x);

/*
\shead	Kopfdeklarationen

Grundsätzlich können Deklarationen zu Beginn einer Funktion
wie gewöhnliche Zeilen definiert werden. Die Funktion |$1| erlaubt
aber, Kopfdeklarationen im laufenden Text zu ergänzen. Alle mit
|$1| eingetragenen Datenzeilen werden vor allen anderen Datenzeilen
ausgegeben.
*/

void Function::head & (str _fmt = NULL, ...)
	_head[_nh++] = { 1, psub(_fmt, va_list) };

/*
\shead	Blockstrukturen

Um einen gut strukturierten Block zu erhalten, sind
vor den Befehlszeilen je nach Blocktiefe entsprechend
viele Tabulatoren einzufügen.

Die nachfolgenden Funktionen erleichtern die zeilenweise Konstruktion
von Blöcken. Die Einrücktiefe wird automatisch adjustiert.

\ttitem	$1
	beginnt einen Funktionsblock. Das Argument <_fmt>
	wird vor der öffnenden Klammer eingefügt.
*/

void Function::beg & (str _fmt = NULL)
	startline(), _body[_nb++] = { depth++, psub(_fmt) + "{" }, ignorepar();

/*
\ttitem	$1
	schließt einen Funktionsblock.
	Das Argument <_fmt> wird nach der schließenden Klammer eingefügt.
*/

void Function::end & (str _fmt = NULL)
	_body[_nb++] += { --depth, "}" + psub(_fmt) }, newpar();

/*
\ttitem	$1
	generiert eine Blockstruktur mit Kopf <head> und Körper <body> 
*/

void Function::block & (str head, str body, ...)
{
	startline();

	if	(head)
		_body[_nb++] += { depth, psub(head, va_list) };

	_body[_nb++] += { depth++, "{" };

	for (x in split(psub(body), "\n"))
		_body[_nb++] += { depth, x };

	_body[_nb++] += { --depth, "}" };
	newpar();
}

/*
\ttitem	$1
	erzeugt den Kopf einer Switch-Anweisung.
*/

void Function::switch & (str _expr, ...)
	line("switch (" + _expr + ")", va_list), beg();

/*
\ttitem	$1
	erzeugt eine |case|-Anweisung mit zugehörigem Kommando <_cmd>.
	Ist <_key> der Null-String, wird eine |default| Anweisung
	generiert. Die |case|-Label stehen auf der gleichen
	Einrücktiefe wie die Blockklammern der |switch|-Struktur.
*/

inline void Function::case & (str _key, str _cmd = NULL, ...)
{
	_cmd = psub(_cmd, va_list);
	_key = _key ? "case " + psub(_key, va_list) : "default";
	add_line(depth - 1, paste("\t", _key + ":", _cmd));
}

/*
\ttitem	$1
	erzeugt eine |case|-Anweisung mit einem Block von
	Befehlszeilen.
*/

inline void Function::cblock & (str _key, str _cmd, ...)
{
	_cmd = psub(_cmd, va_list);
	_key = _key ? "case " + psub(_key, va_list) : "default";

	add_line(depth - 1, _key + ":");

	for (x in split(_cmd, "\n"))
		add_line(depth, x);
}

/*
Einfache C-Funktionen können mit den Funktionen |GlobalFunction| und
|LocalFunction| konstruiert werden. Das Argument <_body> bestimmt den
Funktionsrumpf. Auch auf ihn wird Parametersubstitution angewendet.
Für alle anderen Argumente gilt das bei der Beschreibung
des Konstruktors gesagte.
*/

inline void GlobalFunction (str _name, str _args, str _body, ...)
{
	Function f = Function(_name, _args, va_list);
	f.body(_body, va_list);
	f.write(true);
}

inline void LocalFunction (str _name, str _args, str _body, ...)
{
	Function f = Function(_name, _args, va_list);
	f.body(_body, va_list);
	f.write(false);
}

/*
$Examples

Das erste Beispiel zeigt die Generierung des Kommandos
<"Hello World"> mit Hilfe des Datentypes |Function|.

\ms_example hello
config src 
#include <macro.ms>
#include <function.ms>

StdInclude(hdr, "stdio");

GlobalFunction("int main", "int argc, char **argv", string !
printf("Hello World!\n");
return 0;
!);
\end

Das zweite Beispiel zeigt die Konstruktion eines |switch|-Blocks
mit einzelnen Funktionsaufrufen.

\ms_example switch
config hdr src 
#include <macro.ms>
#include <function.ms>

Include(src, HeaderName);

Function f = Function("int test", "int key, int mode");
f.switch("key");
f.case("1");
f.case("2", "return 1;");
f.cblock("3", string !
if	(mode)	return 2;
else		return 3;
!);
f.case(NULL, "return 0;");
f.end();
f.write(true);
\end

$SeeAlso
\mref{mksource(1)}, \mref{macro(7)}, \mref{printfunc(7)}.
*/

#endif	/* function.ms */
