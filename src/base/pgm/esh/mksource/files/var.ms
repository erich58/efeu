/*	Hilfsfunktionen zur Variablendefinition
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_MS_VAR
#define	_MS_VAR	1

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
	_decl = psub(_decl, va_list) + sprintf("[%d]", _dim);

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

		for (int n = 0; n < _dim; n++)
			src << "\t" << psub(_val) << ",\n";

		src << "}";
	}

	src << ";\n\n";
}

#endif	_MS_VAR
