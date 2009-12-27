/*	Synthesis-Datenbankkonverter
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#ifndef	_synkonv_ms
#define	_synkonv_ms	1

#include <syntools.ms>

str SynKonvFunc (str t1, str t2, str args, str body, bool glob = true)
{
	Function f = Function("void Konv_$1_$2",
		paste(", ", "const $1_t *src, $2_t *tg", args), t1, t2);
	f.body(body);
	f.write(glob);
}

str SynKonvDef (str db1, str db2, str body,
	str par = NULL, str val = NULL, str desc = NULL, bool glob = true)
{
	db1 = psub(db1);
	db2 = psub(db2);
	str name = paste("_to_", db1, db2);
	str info = SynInfo(name, desc);

	setreg(0, name);
	LocalFunction("$2_t *$0_func",
		"SynDBIO_t *base, $2_t *data, $3 *par",
		body, db1, db2, par ? par + "_t" : "void");

	if	(glob)
		fprintf(hdr, "extern SynDBKonvDef_t %s;\n", name);
	else	src << "static ";

	fprintf(src, "SynDBKonvDef_t %s = {\n", name);
	fprintf(src, "\t&%s_DB, &%s_DB,\n", db1, db2);
	fprintf(src, "\t(SynDBKonv_t) %s_func,\n", name);

	if	(par)
	{
		fprintf(src, "\t%s_init, (clean_t) %s_clean,\n", par, par);
	}
	else	src << "\tNULL, NULL,\n";

	fprintf(src, "\t%#s, %s\n};\n", val, info);
	return name;
}

#endif	/* synkonv.ms */
