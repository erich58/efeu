/*	Synthesis - Datenbank
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_MS_SYNDB
#define	_MS_SYNDB

#include <macro.ms>
#include <var.ms>
#include <struct.ms>
#include <function.ms>
#include <printfunc.ms>
#include <bitfield.ms>
#include <dbdata.ms>
#include <syntools.ms>

struct SynDB {
	str type;
	str dbname;
	str version;
	str title;
	DataBase db_idef;	/* Eingabedefinitionen */
	DataBase db_odef;	/* Ausgabedefinitionen */
	DataBase db_test;
	DataBase db_comp;
	DataBase db_klass;
	DataBase db_cnt;
};

SynDB SynDB (str type, str version, str title = NULL)
{
//	name = psub(name);
//	version = strsub(strsub(psub(version), "V"), "R", ".");
	version = psub(version);
	str name = strcut(version, "-");
	type = psub(type);
	title = psub(title);

	return (SynDB) { type, name, version, title,
		DataBase(str), DataBase(str), DataBase(str),
		DataBase(str), DataBase(str), DataBase(str) };
}

/*	Testfunktionen
*/

void SynDB::add_test (str name, str desc, str par, str func)
	db_test += sprintf("\t{ %#s, %#s,\n\t\t%s, %s },\n",
		name, desc, func, (par ? par : "NULL"));

void SynDB::test (str name, str desc, str par, str body, str expr = NULL)
{
	LocalFunction("int test_$1", "void *arg, $2_t *dat, void *par",
		body, name, type, expr);
	add_test(name, desc, psub(par), psub("(SynEvalFunc_t) test_$1"));
}

void SynDB::testflag (str name, str desc, str expr)
	test(name, desc, "NULL", "return (" + expr + ");");

void SynDB::testval (str name, str desc, str expr)
	test(name, desc, "&FilterPar_Num",
		"return TestVal(par, (" + expr + "));");

void SynDB::stich (str name, str desc, str expr)
	test(name, desc, "&FilterPar_Rand",
		"return TestRandPar(par, (" + expr + "));");

void SynDB::codepar (str name, str label, str par)
{
	if	(label)	label = "=" + label;

	fprintf(src, "static SynFilterPar_t %s = { %#s,\n", name, label);
	fprintf(src, "\tMakeCodePar, memfree, %s };\n\n", par);
}

void SynDB::testcode (str name, str desc, str code, str expr)
{
	codepar("par_" + name, "clist", psub(code));
	test(name, desc, "&par_$1", "return TestCode(par, " + expr + ");");
}

/*	Hilfsprogramme für Eingabedefinitionen
*/

inline void SynDB::add_idef (str name, str label, str scan, str par, str desc)
{
	str info = SynInfo(name, desc);
	str args;

	if	(par)
	{
		args = sprintf("%s_init, (clean_t) %s_clean", par, par);
	}
	else	args = "NULL, NULL";

	db_idef += sprintf("\t{ %#s, %#s,\n\t\t%s, %s, %s },\n",
		name, label, scan, args, info);
}

void SynDB::input (str name, str label, str def, str par = NULL)
{
	LocalFunction("int scan_" + name,
		"io_t *io, void *data, void *ptr", def);
	add_idef(name, label, "scan_" + name, par, lastcomment());
}


/*	Hilfsprogramme für Ausgabedefinitionen
*/


void SynDB::add_odef (str name, str desc, str create, str par)
	db_odef += sprintf("\t{ %#s, %#s,\n\t\t%s, %s, %s },\n",
		name, desc, create, (par ? par : "NULL"));

void SynDB::output (str name, str label, str def, str desc)
{
	str info = SynInfo(name, desc);
	LocalFunction("SynDBIO_t *out_" + name, "SynDBType_t *type, \
const char *name, io_t *io, const char *hdr, void *par, \
const char *opt, const char *arg", def);
	add_odef(name, label, "out_" + name, info);
}

void SynDB::flagdef (str name, IO io)
{
	str f, k, d;

	fprintf(src, "static SynDBFlagDef_t flag_%s[] = {\n", name);

	while (getline(io, f, k, d))
		fprintf(src, "\t{ %s, %#c, %#s },\n", f, k[0], d);

	src << "};\n\n";
	close(io);
}


/*	Aufspaltungsdefinition
*/

void SynDB::add_split (str name, str desc)
{
	setreg(1, name);
	fpsub(src, "static SynDBSplitDef_t odef_$1 = {\n");
	fpsub(src, "\t&SynDBSplit_info, (SynDBSplit_t) $1_idx, $1_dim, $1_tab\n};\n\n");
	add_odef(name, desc, "SynDBSplit", "&odef_" + name);
}


/*	Darstellungsfunktionen
*/

void SynDB::add_pdef (str name, str desc, str fdef)
{
	setreg(1, name);

	if	(fdef)
	{
		flagdef(name, fdef);
		setreg(2, psub("flag_$1, tabsize(flag_$1)"));
	}
	else	setreg(2, "NULL, 0");

	fpsub(src, "static SynDBPrintDef_t odef_$1 = {\n");
	fpsub(src, "\t&SynDBPrint_info, (SynDBPrint_t) print_$1, $2\n};\n\n");
	add_odef(name, desc, "SynDBPrint", "&odef_" + name);
}

void SynDB::print (str name, str desc, str body, str par = NULL)
{
	Function f = Function("int print_$1",
		"io_t *out, $2_t *dat, unsigned flag", name, type);
	f.body(body);
	f.write(false);
	add_pdef(name, desc, par);
}

void SynDB::pfunc (str name, str desc, IO def, str par = NULL)
{
	Function f = Function("int print_$1",
		"io_t *out, $2_t *dat, unsigned flag", name, type);
	f.pfbody("out", def);
	f.write(false);
	add_pdef(name, desc, par);
}

/*	Kopierfunktionen
*/

void SynDB::cpfunc (str name, str desc, str type, str func,
	str init = "NULL", str close = "NULL")
{
	setreg(1, psub(name));
	setreg(2, psub(type));
	setreg(3, psub(func));
	str info = SynInfo(name, lastcomment());

	fpsub(src, "static SynDBCopyDef_t odef_$1 = {\n");
	fpsub(src, "\t&SynDBCopy_info, &$2_DB, $3,\n");
	fprintf(src, "\t%s, %s, %s\n};\n\n", info, psub(init), psub(close));
	add_odef(name, desc, "SynDBCopy", "&odef_" + name);
}

/*	Transformationsfunktion
*/

void SynDB::trans (str name, str desc, str type, str body,
	str par = NULL, str args = NULL)
{
	name = psub(name);
	type = psub(type);
	str info = SynInfo(name, lastcomment());

	LocalFunction("int $1_func", "io_t *out, $2_t *data, $3 *par",
		body, name, type, par ? par + "_t" : "void");

	fprintf(src, "static SynDBTransDef_t trans_%s = {\n", name);
	fprintf(src, "\t&SynDBTrans_info, (SynDBTrans_t) %s_func,\n", name);
	fprintf(src, "\t%#s, %s,\n", args, info);

	if	(par)
	{
		fprintf(src, "\t(void *) %s_init, (void *) %s_clean,\n",
			par, par);
	}
	else	src << "\tNULL, NULL,\n";

	src << "\n};\n";
	add_odef(name, desc, "SynDBTrans", "&trans_" + name);
}

/*	Vergleichsfunktionen
*/

void SynDB::add_comp (str name, str desc, str size, str set, str func)
	db_comp += sprintf("\t{ %#s, %#s,\n\t\t%s, %s, %s },\n",
		name, desc, size, set, func);

void SynDB::comp (str name, str desc, IO def)
{
	str fname = sprintf("comp_%s", name);
	CompFunc(fname, type, def, false);
	add_comp(name, desc, "0", "NULL", "(comp_t) " + fname);
}

void SynDB::keycomp (str name, str desc, IO def)
{
	setreg(1, name);
	setreg(2, type);
	Function f_key = Function("void key_$1",
		"key_$1_t *key, $2_t *dat");
	Function f_comp = Function("int comp_$1",
		"const key_$1_t *a, const key_$1_t *b");

	fpsub(src, "typedef struct {\n");
	str tname, vname, key, cmd;

	while (getline(def, tname, vname, key, cmd))
	{
		f_key.line("key->$1 = " + cmd + ";", vname);

		if	(isdigit(tname[0]))
		{
			fprintf(src, "\tunsigned %s : %d;\n", vname, tname);
		}
		else	fprintf(src, "\t%s %s;\n", tname, vname);

		if	(key == "std")
		{
			f_comp.line("if\t(a->$1 < b->$1)\treturn -1;");
			f_comp.line("if\t(a->$1 > b->$1)\treturn 1;");
		}
		else if	(key == "rev")
		{
			f_comp.line("if\t(a->$1 > b->$1)\treturn -1;");
			f_comp.line("if\t(a->$1 < b->$1)\treturn 1;");
		}
	}

	f_comp.line("return 0;");

	fpsub(src, "} key_$1_t;\n\n", name);
	f_key.write(false);
	f_comp.write(false);
	add_comp(name, desc, psub("sizeof(key_$1_t)"),
		psub("(SynDBSet_t) key_$1"), psub("(comp_t) comp_$1"));
}

void SynDB::stdcomp (str name, str desc, str def)
{
	str fname = sprintf("comp_%s", name);
	Function f = Function("int $1", "const $2_t *a, const $2_t *b",
		fname, type);

	for (x in split(def, "%s,"))
	{
		if	(strlen(x) == 0)	continue;

		if	(x[0] == "-")
		{
			setreg(1, substr(x, 1));
			f.line("if\t(a->$1 > b->$1)\treturn -1;");
			f.line("if\t(a->$1 < b->$1)\treturn 1;");
		}
		else
		{
			setreg(1, (x[0] == '+' ? substr(x, 1) : x));
			f.line("if\t(a->$1 < b->$1)\treturn -1;");
			f.line("if\t(a->$1 > b->$1)\treturn 1;");
		}
	}

	f.line("return 0;");
	f.write(false);
	add_comp(name, desc, "0", "NULL", "(comp_t) " + fname);
}

void SynDB::mcomp (str name, str desc, str def)
{
	str fname = sprintf("comp_%s", name);
	Function f = Function("int $1", "const $2_t *a, const $2_t *b",
		fname, type);

	for (x in split(psub(def), "%s,"))
	{
		if	(strlen(x) == 0)	continue;

		if	(x[0] == "-")
		{
			setreg(1, substr(x, 1));
			f.line("if\t($1(a) > $1(b))\treturn -1;");
			f.line("if\t($1(a) < $1(b))\treturn 1;");
		}
		else
		{
			setreg(1, (x[0] == '+' ? substr(x, 1) : x));
			f.line("if\t($1(a) < $1(b))\treturn -1;");
			f.line("if\t($1(a) > $1(b))\treturn 1;");
		}
	}

	f.line("return 0;");
	f.write(false);
	add_comp(name, desc, "0", "NULL", "(comp_t) " + fname);
}

/*	Aufspaltungsdefinitionen
*/

void SynDB::add_klass (str name, str desc, str anz, str func)
	db_klass += sprintf("\t{ %#s, %#s,\n\t\t%s, %s },\n",
		name, desc, anz, func);

void SynDB::klass (str name, str desc, str anz, str body)
{
	str fname = sprintf("split_%s", name);
	Function f = Function("int $1", "$2_t *dat, int dim",
		fname, type);
	f.body(body);
	f.write(false);
	add_klass(name, desc, anz, "(SynDBClass_t) " + fname);
}

/*	Auszähldefinitionen
*/

void SynDB::cntinit(...)
	for (x in va_list) db_cnt += psub(x) + "(eval, count, class)";

void SynDB::cnteval(...)
	for (x in va_list) db_cnt += psub(x) + "(eval)";

void SynDB::cntdef(...)
	for (x in va_list) db_cnt += psub(x) + "(count)";

void SynDB::cntclass(...)
	for (x in va_list) db_cnt += psub(x) + "(class)";

/*	Konstruktion des Datenbanktypes
*/

str SynDB::db_tab(str type, str name, DataBase db)
{
	if	(!dim(db))	return "NULL, 0";

	fpsub(src, "\nstatic $1 $2[] = {\n", type, name);

	for (x in db)
		src << x;

	src << "};\n\n";
	return psub("$1, tabsize($1)", name);
}

void SynDB::create (bool glob = true)
{
	Function f;

	str tab = "\t";
	tab += db_tab("SynDBInput_t", "tab_idef", db_idef);
	tab += ",\n\t";
	tab += db_tab("SynDBOutput_t", "tab_odef", db_odef);
	tab += ",\n\t";
	tab += db_tab("SynEval_t", "tab_test", db_test);
	tab += ",\n\t";
	tab += db_tab("SynDBComp_t", "tab_sort", db_comp);
	tab += ",\n\t";
	tab += db_tab("SynDBOldSplit_t", "tab_split", db_klass);
	tab += ",\n";

	str name = type + "_DB";
	setreg(1, name);
	setreg(2, type);

	LocalFunction("size_t $1_read", "io_t *io, void *data",
		"return $2_read(io, data, 1);");
	LocalFunction("size_t $1_write", "io_t *io, void *data",
		"return $2_write(io, data, 1);");
	LocalFunction("void $1_clear", "void *data",
		"$2_clear(data, 1);");

	str cntinit;

	if	(dim(db_cnt))
	{
		cntinit = psub("$1_cntinit");

		f = Function("void $1_cntinit",
			"xtab_t *eval, xtab_t *count, xtab_t *class");

		for (x in db_cnt)
			f.fline("%s;", x);

		f.write(false);
	}
	else	cntinit = "NULL";

	f = Function("void $1_info", "InfoNode_t *info");
	f.fline("SynDBDocInfo(info, %#s);", BaseName + ".info");
	f.write(false);

	if	(glob)
		fpsub(hdr, "\nextern SynDBType_t $1;\n", name);
	else	src << "static ";

	fpsub(src, "SynDBType_t $1 = {\n");
	fprintf(src, "\t%#s, %#s, %#s,\n", dbname, version, title);
	fpsub(src, "\t$2_recl, $2_size,\n");
	fpsub(src, "\t$1_read, $1_write, $1_clear,\n");
	fpsub(src, "\tNULL,\n");
	src << tab;
	fprintf(src, "\t%s,\n", cntinit);
	fpsub(src, "\t$1_info,\n");
	src << "};\n";
}

#endif	/* _MS_SYNDB */
