/*	Synthesis-Datenbankkonverter
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#ifndef	_synjoin_ms
#define	_synjoin_ms	1

#include <syntools.ms>

str SynJoinDef (str db0, str db1, str db2, str body,
	str par = NULL, str val = NULL, str desc = NULL, bool glob = true)
{
	db1 = psub(db1);
	db2 = psub(db2);
	str name = psub("Join_$1_$2", db1, db2);
	str info = SynInfo(name, desc);

	setreg(0, name);
	LocalFunction("$3_t *$0_func",
		"SynDBIO_t *db1, SynDBIO_t *db2, $3_t *ptr, $4 *par",
		body, db1, db2, db0, par ? par + "_t" : "void");

	if	(glob)
		fprintf(hdr, "extern SynDBJoinDef_t %s;\n", name);
	else	src << "static ";

	fprintf(src, "SynDBJoinDef_t %s = {\n", name);
	fprintf(src, "\t&%s_DB, &%s_DB, &%s_DB,\n", db0, db1, db2);
	fprintf(src, "\t(SynDBJoin_t) %s_func,\n", name);

	if	(par)
	{
		fprintf(src, "\t%s_init, (clean_t) %s_clean,\n", par, par);
	}
	else	src << "\tNULL, NULL,\n";

	fprintf(src, "\t%#s, %s\n};\n", val, info);
	return name;
}

#endif	/* synjoin.ms */
