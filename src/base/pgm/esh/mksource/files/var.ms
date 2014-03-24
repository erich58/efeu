/*
Hilfsfunktionen zur Variablendefinition

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

#ifndef	var_ms
#define	var_ms	1

/*
Die Deklarationsfunktionen sind vom Type inline, damit bei der
Parametersubstitution der Argumente auch lokale Variablen sichtbar
bleiben. Die internen Namen werden zur Vermeidung von
Sichtbarkeitskonflikten mit "_" gekennzeichnet.

Die Funktion |$1| richtet eine Variable ein. Falls <_glob> den Wert
true hat, wird sie mit externer Verknüpfbarkeit und ansonsten
statisch eingerichtet.
*/

inline void DeclVar (bool _glob = true, str _decl,
	str _val = NULL, str _desc = NULL, ...)
{
	_decl = psub(_decl, va_list);

	if	(_val)	_val = " = " + psub(_val, va_list);
	if	(_desc)	_desc = "\t/* " + psub(_desc, va_list) + " */";

	if	(_glob)
		fprintf(hdr, "extern %s;%s\n", _decl, _desc);
	else	src << "static ";

	fprintf(src, "%s%s;%s\n", _decl, _val, _desc);
}

/*
Die Funktion |$1| richtet eine Variable mit externer Verknüpfbarkeit ein.
*/

void GlobalVar (str _decl, str _val = NULL, str _desc = NULL, ...)
	DeclVar(true, _decl, _val, _desc, va_list);

/*
Die Funktion |$1| richtet eine statische Variable ein.
*/

void LocalVar (str _decl, str _val = NULL, str _desc = NULL, ...)
	DeclVar(false, _decl, _val, _desc, va_list);


/*
Die Funktion |$1| richtet einen Vektor ein. Falls <glob> den Wert
true hat, wird der Vektor mit externer Verknüpfbarkeit und ansonsten
statisch eingerichtet. Falls für |_dim| der Wert 0 übergeben wird,
bestimmt |_val| die Zuweisungsliste für den Vektor. Bei jedem
Zeilenumbruch in |_val| wird ein Komma eingefügt. Ist |_dim| größer als 0,
steht |_val| für jedes einzelne Vektorelement. Der Laufindex |n| kann bei
der Wertedefinition verwendet werden.
*/

inline void DeclVec (bool _glob = true, str _decl, int _dim,
	str _val = NULL, str _desc = NULL, ...)
{
	_decl = psub(_decl, va_list) + sprintf("[%.0d]", _dim);

	src << "\n";

	if	(_desc)
	{
		_desc = psub(_desc, va_list);

		if	(_glob)
			fprintf(hdr, "extern %s;\t/* %s */\n", _decl, _desc);

		fprintf(src, "/*\n%s\n*/\n", _desc);
	}
	else if	(_glob)
	{
		fprintf(hdr, "extern %s;\n", _decl);
	}

	if	(!_glob)
		src << "static ";

	src << _decl;

	if	(_val)
	{
		src << " = {\n";

		if	(_dim)
		{
			for (int n = 0; n < _dim; n++)
				src << "\t" << psub(_val, va_list) << ",\n";
		}
		else
		{
			for (x in split(psub(_val, va_list), "\n"))
				src << "\t" << x << ",\n";
		}

		src << "}";
	}

	src << ";\n\n";
}

/*
Die Funktion |$1| richtet einen Vektor mit externer Verknüpfbarkeit ein.
*/

void GlobalVec (str _decl, int _dim, str _val = NULL, str _desc = NULL, ...)
	DeclVec(true, _decl, _dim, _val, _desc, va_list);

/*
Die Funktion |$1| richtet einen statischen Vektor ein.
*/

void LocalVec (str _decl, int _dim, str _val = NULL, str _desc = NULL, ...)
	DeclVec(false, _decl, _dim, _val, _desc, va_list);

/*
$Example
Das folgende Beispiel zeigt die Verwendung der Deklarationsfunktionen:

\ms_example	var
config hdr src

#include <var.ms>
GlobalVar("int x", NULL, "Variable x");
LocalVar("int \$1_x", "42", "Variable \$1_x", "pfx");

GlobalVec("struct {
	double n, l;
} tab", 5, "{ \$(n + 1.:%#.0f), \$(log(n + 1.):%.8f) }", "Log-Table");

LocalVec("int x", 0, string !
23, 14
39
!, "Vektor x");
\end

$SeeAlso
\mref{mksource(1)}.
*/

#endif	/* var.ms */
