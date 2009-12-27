/*
Hilfsfunktionen zur Variablendefinition

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

/*	Die Deklarationsfunktionen sind vom Type inline, damit bei der
	Parametersubstitution der Argumente auch lokale Variablen sichtbar
	bleiben. Die internen Namen werden zur Vermeidung von
	Sichtbarkeitskonflikten mit "_" gekennzeichnet.
*/

inline void DeclVar (bool _glob = true, str _decl,
	str _val = NULL, str _desc = NULL, ...)
{
	_decl = psub(_decl, va_list);

	if	(_val)	_val = " = " + psub(_val);
	if	(_desc)	_desc = "\t/* " + psub(_desc) + " */";

	if	(_glob)
		fprintf(hdr, "extern %s;%s\n", _decl, _desc);
	else	src << "static ";

	fprintf(src, "%s%s;%s\n", _decl, _val, _desc);
}

void GlobalVar (str _decl, str _val = NULL, str _desc = NULL, ...)
	DeclVar(true, _decl, _val, _desc, va_list);

void LocalVar (str _decl, str _val = NULL, str _desc = NULL, ...)
	DeclVar(false, _decl, _val, _desc, va_list);


/*	Bei der Vektordefinition kann der Laufindex n in der Wertedefinition
	verwendet werden.
*/

inline void DeclVec (bool glob = true, str _decl, int _dim,
	str _val = NULL, str _desc = NULL, ...)
{
	_decl = psub(_decl, va_list) + sprintf("[%.0d]", _dim);

	if	(_desc)
	{
		_desc = "/*\t" + psub(_desc) + "\n*/\n";

		if	(glob)	hdr << _desc;

		src << _desc;
	}

	src << "\n";

	if	(glob)
		fprintf(hdr, "extern %s;\n", _decl);
	else	src << "static ";

	src << _decl;

	if	(_val)
	{
		src << " = {\n";

		if	(_dim)
		{
			for (int n = 0; n < _dim; n++)
				src << "\t" << psub(_val) << ",\n";
		}
		else
		{
			for (x in split(psub(_val), "\n"))
				src << "\t" << x << ",\n";
		}

		src << "}";
	}

	src << ";\n\n";
}

void GlobalVec (str _decl, int _dim, str _val = NULL, str _desc = NULL, ...)
	DeclVec(true, _decl, _dim, _val, _desc, va_list);

void LocalVec (str _decl, int _dim, str _val = NULL, str _desc = NULL, ...)
	DeclVec(false, _decl, _dim, _val, _desc, va_list);

#endif	/* var.ms */
