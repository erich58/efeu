/*	Aufzählungen
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_Enum_ms
#define	_Enum_ms	1

#include <function.ms>

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
	fprintf(src, "static char *%sTab[] = {\n", ename);

	for (n = 0; getline(def, name, desc); n++)
	{
		if	(patcmp("<1->[%n_]", name))
		{
			fprintf(io, "#define\t%s_%s\t%d", ename, name, n);
			fprintf(io, "\t/* %s */\n", desc);
		}

		fprintf(src, "\t%#s,\t/* %s */\n", name, desc);
	}

	if	(n == 0)
		fprintf(src, "\t%#s,\t/* %s */\n", NULL, "Dummyeintrag");

	fprintf(io, "\n#define\t%sDim\t%d\t/* Zahl der Ausprägungen */\n\n",
		ename, n);

	src << "};\n";

	f = Function("unsigned $1Index", "const char *name", ename);
	f.line("register int i;");
	f.line();
	f.line("for (i = 0; i < $1Dim; i++)");
	f.line("\tif (mstrcmp(name, $1Tab[i]) == 0) return i;");
	f.line();
	f.line("return 0;");
	f.write(glob);

	f = Function("char *$1Label", "unsigned n", ename);
	f.line("return $1Tab[n < $1Dim ? n : 0];");
	f.write(glob);
}

void ListEnum (str ename, str label, bool glob, ...)
{
	str name, desc;
	Function f;
	int n;
	IO io = glob ? hdr : src;

	if	(glob)
		fprintf(hdr, "\n/*\t%s\n*/\n\n", label);

	fprintf(src, "\n/*\t%s\n*/\n\n", label);
	fprintf(src, "static char *%sTab[] = {\n", ename);
#if	1
	fprintf(src, "\t%#s,", NULL);
	n = 1;
#else
	n = 0;
#endif

	for (name in va_list)
	{
		if	(patcmp("<1->[%n_]", name))
			fprintf(io, "#define\t%s_%s\t%d\n", ename, name, n);

		if	(n % 4 == 0)
			src << "\n";

		fprintf(src, "\t%#s,", name);
		n++;
	}

	src << "\n};\n";

	fprintf(io, "\n#define\t%sDim\t%d\t/* Zahl der Ausprägungen */\n\n",
		ename, n);

	f = Function("unsigned $1Index", "const char *name", ename);
	f.line("register int i;");
	f.line();
	f.line("for (i = 1; i < $1Dim; i++)");
	f.line("\tif (mstrcmp(name, $1Tab[i]) == 0) return i;");
	f.line();
	f.line("return 0;");
	f.write(glob);

	f = Function("char *$1Label", "unsigned n", ename);
	f.line("return $1Tab[n < $1Dim ? n : 0];");
	f.write(glob);
}

#endif	/* _Enum_ms */
