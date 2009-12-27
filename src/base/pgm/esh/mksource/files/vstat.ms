/*	Verarbeitungsstatistik
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_vstat_ms
#define	_vstat_ms	1

void VStat(str sname, str label, IO def, bool glob = true)
{
	str name, fmt;
	Function f;
	int n;
	IO io = glob ? hdr : src;

	sname = psub(sname);
	label = psub(label);

	if	(glob)
		fprintf(hdr, "\n/*\t%s\n*/\n\n", label);

	fprintf(src, "\n/*\t%s\n*/\n\n", label);
	fprintf(src, "char *%s_fmt[] = {\n", sname);

	for (n = 0; getline(def, name, fmt); n++)
	{
		name = psub(name, sname);

		if	(patcmp("<1->[%n_]", name))
		{
			fprintf(io, "#define %s\t%d", name, n);
			fprintf(io, "\t/* %#s */\n", fmt);
		}

		fprintf(src, "\t%#s,\n", fmt);
	}

	if	(n == 0)
	{
		fprintf(src, "\t%#s,\t/* %s */\n", NULL, "Dummyeintrag");
		n++;
	}

	src << "};\n";

	fprintf(io, "\n#define %s_dim\t%d\t/* Zahl der Formate */\n\n",
		sname, n);
	fprintf(io, "extern char *%s_fmt[%d];\t/* Formatvektor */\n",
		sname, n);
}

#endif	/* _vstat_ms */
