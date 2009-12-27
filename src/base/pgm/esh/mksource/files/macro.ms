/*	Hilfsfunktionen zur Makrogenerierung
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_MS_MACRO
#define	_MS_MACRO	1

void Macro(IO io, str name, str args, str def, ...)
{
	name = psub(name, va_list);
	args = strlen(args) ? psub(args) : args;
	def = psub(def);
	fprintf(io, "#define\t%s", name);

	if	(args)
		fprintf(io, "(%s)", args);

	str delim = "\t";

	for (x in split(def, "\n"))
	{
		if	(strlen(x))
		{
			io << delim;
			io << x;
			delim = "\t\\\n\t";
		}
	}

	io << "\n";
}

void Define(IO io, IO def, ...)
{
	fpsub(io, "\n", va_list);

	str name, desc;

	while (getline(def, name, desc))
	{
		io << "#define\t";
		fpsub(io, name);
		io << "\t";
		fpsub(io, xstrcut(desc, "%s"));

		if	(strlen(desc))
		{
			io << "\t/* ";
			fpsub(io, desc);
			io << " */";
		}

		io << '\n';
	}

	io << '\n';
}

void Include(IO io, str def, ...)
{
	fpsub(io, "\n", va_list);

	for (x in split(psub(def, va_list), "%s"))
	{
		if	(strlen(x) == 0)	continue;

		io << "#include ";
		fprintf(io, (x[0] == '<' ? "%s" : "%#s"), x);
		io << '\n';
	}

	io << '\n';
}

void StdInclude (IO io, ...)
{
	for (x in va_list)
		fprintf(io, "#include <%s.h>\n", x);

	io << '\n';
}

#endif	_MS_MACRO
