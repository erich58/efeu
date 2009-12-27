/*	Datenbank - Datenfelder
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_MS_DBDATA
#define	_MS_DBDATA	1

#include <var.ms>
#include <macro.ms>
#include <function.ms>

/*	Datenstrukturvariablen
*/

struct DBDataVar {
	str base;	/* Basisname des Types */
	str type;	/* Variablentype */
	str name;	/* Variablenname */
	str desc;	/* Beschreibungstext */
	str vdim;	/* Vektordimension */
};

void DBDataVar::typeline(IO io)
{
	if	(name == NULL)	return;

	if	(base)	fprintf(io, "\t%s_t ", base);
	else if	(type)	fprintf(io, "\t%s ", type);
	else		io << '\t';

	if	(vdim)	io << '*';

	fprintf(io, "%s;", name)

	if	(desc)
		fprintf(io, "\t/* %s */", desc);

	io << '\n';
}


/*	Datendefinition
*/

struct DBDataDef {
	str name;	/* Datentypename */
	str label;	/* Beschreibungstext */
	bool glob;	/* Globale Definition */
	bool fix;	/* Fixe Satzlänge */

	DataBase var;	/* Variablentabelle */
	DataBase slist;	/* Synchronisationsliste */
	str recl;	/* Satzlänge */

	str argdecl;	/* Zusätzliche Argumente beim Lesen/Synchronisieren */
	str arglist;	/* Zusätzliche Argumente beim Lesen/Synchronisieren */
	str varlist;	/* Zusätzliche Variablen beim Lesen/Synchronisieren */
	str syncvar;	/* Zusätzliche Variablen beim Synchronisieren */

	str read_body;	/* Funktionsrumpf der Eingabefunktion */
	str write_body;	/* Funktionsrumpf der Schreibfunktion */
	str alloc_body;	/* Funktionsrumpf der Zuweisungsfunktion */
	str clear_body;	/* Funktionsrumpf der Löschfunktion */

	str space_body;	/* Funktionsrumpf der Speicherbedarfsfunktion */
	str sync_head;	/* Funktionskopf zur Synchronisation */
	str sync_body;	/* Funktionsrumpf zur Synchronisation */
	str sync_foot;	/* Funktionsfuß zur Synchronisation */
};

DBDataDef DBDataDef(str name, str label, bool glob = true)
{
	name = psub(name);
	label = psub(label);

	if	(!label)	label = psub("Datenstruktur $1_t", name);
	if	(glob)		fprintf(hdr, "\n/*\t%s\n*/\n", label);

	fprintf(src, "\n/*\t%s\n*/\n\n", label);
	return { name, label, glob, true,
		DataBase(DBDataVar),
		DataBase(str, cmp),
		};
}

/*	Datentype generieren
*/

void DBDataDef::typedef(bool flag = true)
{
	IO io = flag ? hdr : src;
	int n = dim(var);
	int i;

	io << "\ntypedef struct {\n";

	for (i = 0; i < n; i++)
		var[i].typeline(io);

	fprintf(io, "} %s_t;\n\n", name);
	Macro(io, "$1_recl", NULL, (fix ? "($2)" : "0"), name, recl);
	Macro(io, "$1_size", NULL, "sizeof($1_t)", name, recl);

	if	(!read_body)
		Macro(io, "$1_read", "io, data, dim",
			"io_dbread(io, data, $1_recl, $1_size, dim)");
	
	if	(!write_body)
		Macro(io, "$1_write", "io, data, dim",
			"io_dbwrite(io, data, $1_recl, $1_size, dim)");

	if	(!clear_body)
		Macro(io, "$1_clear", "data, dim",
			"memset(data, 0, (dim) * $1_size)");

	io << "\n";
}

/*	Datentypefunktionen generieren
*/

void DBDataDef::makefunc(bool glob = true)
{
	Function f;
	setreg(1, name);

	str arg_io = "io_t *io, $1_t *data, size_t n";
	str arg_reorg = "$1_t *data, size_t n";
	str var_reorg = paste(", ", "data, n", arglist);

	str space_set;
	str sync_copy;

	for (x in var)
	{
		if	(!x.vdim)	continue;

		setreg(2, x.name);
		setreg(3, "data->" + x.vdim);

		if	(slist[x.name])
		{
			space_set += psub("$3 = $2_buf - data->$2;\n");
		}
		else
		{
			sync_copy += psub(string !

if	(data->$2)
{
	for (i = 0; i < $3; i++)
		$2_buf[i] = data->$2[i];
}
else	memset($2_buf, 0, $3 * sizeof($2_buf[0]));
!);
		}
	}

/*	Eingabefunktion
*/
	if	(read_body)
	{
		f = Function("size_t $1_read", paste(", ", arg_io, argdecl));
		f.body(varlist + "register size_t s;");
		f.line();
		f.line("for (s = 0; n-- > 0; data++)");
		f.beg();
		f.body(read_body);
		f.end();
		f.line();
		f.line("return s;");
		f.write(glob);
	}

/*	Ausgabefunktion
*/
	if	(write_body)
	{
		f = Function("size_t $1_write", arg_io);
		f.line("register size_t s = 0;");
		f.line();
		f.line("for (s = 0; n-- > 0; data++)");
		f.beg();
		f.body(write_body);
		f.end();
		f.line("return s;");
		f.write(glob);
	}

/*	Aufräumen
*/
	if	(clear_body)
	{
		f = Function("void $1_clear", "$1_t *data, size_t n");
		f.body("while (n-- > 0)");
		f.beg();
		f.body(clear_body);
		f.line("memset(data, 0, sizeof($1_t));");
		f.body("data++;");
		f.end();
		f.write(glob);
	}

/*	Datenreorganisation
*/
	if	(alloc_body)
	{
		f = Function("void $1_alloc", arg_reorg);
		f.line("for (; n-- > 0; data++)");
		f.beg();
		f.body(alloc_body);
		f.end();
		f.write(glob);
	}

/*	Teilvektoren synchronisieren
*/
	if	(space_body)
	{
		f = Function("void $1_space", paste(", ", arg_reorg, argdecl));
		f.body(varlist);
		f.line()
		f.line("for (; n-- > 0; data++)");
		f.beg();
		f.body(space_body);
		f.body(space_set);
		f.end();
		f.write(glob);
	}
	else	Macro(src, "$1_space", "data, dim", NULL);

	if	(sync_copy)
		syncvar += "register size_t i;\n";
		
	if	(sync_body)
	{
		f = Function("void $1_sync", paste(", ", arg_reorg, argdecl));
		f.body(varlist + syncvar);
		f.line("for (; n-- > 0; data++)");
		f.beg();
		f.body(sync_head);
		f.body(sync_copy);
		f.body(sync_body);
		f.body(sync_foot);
		f.end();
		f.write(glob);
	}
	else	Macro(src, "$1_sync", "data, dim", NULL);
}


/*	Datentypebeschreibungstabelle generieren
*/

void DBDataDef::typeinfo()
{
	fpsub(info, "@$1_t:$2\n", name, label);

	for (x in var)
		fprintf(info, "%s %s\t%s\n", x.type, x.name, x.desc)

	info << '\n';
}


/*	Standardvariable
*/

void DBDataDef::std & (str type, str name, str desc)
{
	var += { NULL, type, name, desc };
	setreg(3, psub("sizeof($1)", type, name));
	read_body += psub("s += io_dbread(io, &data->$2, $3, $3, 1);\n");
	write_body += psub("s += io_dbwrite(io, &data->$2, $3, $3, 1);\n");

	if	(fix)
		recl = paste(" + ", recl, psub("$3"));
}


void DBDataDef::ext & (str type, str name, str desc)
	var += { NULL, (type == "." ? NULL : type), name, desc };

/*	Teildatentype mit Standardnamenskonvention
*/

void DBDataDef::sub & (str base, str ndef, str desc, bool flag)
{
	str vname = xstrcut(ndef, ",");

	setreg(0, name);	/* Datentypename */
	setreg(1, base);	/* Basisname des Variablentypes */
	setreg(2, vname);	/* Variablenname */

	var += { base, base + "_t", vname, desc };

	str xarg;

	for (x in split(ndef, ","))
	{
		slist += x;
		xarg += sprintf(", &%s_buf", x);
	}

	setreg(3, "1");
	setreg(5, xarg);

	if	(ndef)
	{
		space_body += psub("$1_space(&data->$2, $3$5);\n");
		sync_body += psub("$1_sync(&data->$2, $3$5);\n");
	}

	read_body += psub("s += $1_read(io, &data->$2, $3$5);\n");
	write_body += psub("s += $1_write(io, &data->$2, 1);\n");

	if	(flag)
	{
		clear_body += psub("$1_clear(&data->$2, 1);\n");
		fix = false;
	}
	else if	(fix)
	{
		recl = paste(" + ", recl, psub("$1_recl"));
	}
}

void DBDataDef::cond & (str key, str base, str name, str desc)
{
	var += { base, base + "_t", name, desc };

	str expr = xstrcut(key, "=");
	str x = paste("->", "data", xstrcut(expr, ":"));
	expr = expr ? psub(expr, x) : x;

	str beg_key, else_key, end_key;

	if	(key)
	{
		beg_key = psub("\nswitch ($1)\n{\n", expr);

		while (key)
			beg_key += psub("case $1:\n", xstrcut(key, ","));

		else_key = "\tbreak;\ndefault:\n";
		end_key = "\tbreak;\n}\n\n";
	}
	else
	{
		beg_key = psub("\nif\t($1)\n", expr);
		else_key = "else";
		end_key = "\n\n";
	}

	setreg(1, base);
	setreg(2, name);
	read_body += psub(beg_key + "\ts += $1_read(io, &data->$2, 1);\n");
	read_body += psub(else_key + "\t$1_clear(&data->$2, 1);\n");
	read_body += end_key;
	write_body += psub(beg_key + "\ts += $1_write(io, &data->$2, 1);\n");
	write_body += end_key;
	fix = false;
	recl = NULL;
}


void DBDataDef::vec & (str key, str base, str ndef, str desc,
	bool iofunc, bool flag)
{
	str vecdim = strcut(key, ",");

	if	(vecdim == NULL)	continue;

	str vname = xstrcut(ndef, ",");

	var += { base, base + "_t", vname, desc, vecdim };

	setreg(0, name);	/* Datentypename */
	setreg(1, base);	/* Basisname des Variablentypes */
	setreg(2, vname);	/* Variablenname */
	setreg(3, "data->" + vecdim);	/* Vektordimension */

	Function f;
	str clear;

	varlist += psub("$1_t *$2_buf;\n");

	if	(key)
	{
		str vecsize = vname + "_size";
		var += { NULL, "size_t", vecsize, NULL };
		setreg(4, "data->" + vecsize);
		setreg(5, key);

		if	(flag)
			clear = "$1_clear(data->$2, $4);\n";

		clear += psub("lfree(data->$2);\n");

		str realloc = "$4 = sizealign($3, $5);";
		realloc += "\ndata->$2 = lmalloc($4 * sizeof($1_t));";

		if	(flag)
			realloc += "\nmemset(data->$2, 0, $4 * sizeof($1_t));";

		f = Function("void $0_$2_realloc", "$0_t *data");
		f.line("if\t($3 > $4)");
		f.beg();
		f.body(clear);
		f.body(realloc);
		f.end();
		f.write(false);

		clear_body += clear;
		alloc_body += psub("$0_$2_realloc(data);\n");
		read_body += psub("$0_$2_realloc(data);\n");
		syncvar += psub("$1_t *$2_save;\n");

		sync_head += psub("\n$2_save = $4 ? data->$2 : NULL;\n");
		sync_head += psub("$4 = sizealign($3, $5);\n");
		sync_head += psub("$2_buf = lmalloc($4 * sizeof($1_t));\n");
		sync_foot += psub("lfree($2_save);\n");
	}
	else
	{
		argdecl = paste(", ", argdecl, psub("$1_t **$2_ptr"));
		arglist = paste(", ", arglist, psub("$2_ptr"));

		space_body += psub("*$2_ptr += $3;\n");
		read_body += psub("data->$2 = *$2_ptr;\n*$2_ptr += $3;\n");
		sync_head += psub("\n$2_buf = *$2_ptr;\n*$2_ptr += $3;\n");
	}

	read_body += psub("$2_buf = data->$2;\n");
	space_body += psub("$2_buf = data->$2;\n");
	sync_body += psub("data->$2 = $2_buf;\n");

	str xarg;

	for (x in split(ndef, ","))
	{
		slist += x;
		xarg += sprintf(", &%s_buf", x);
	}

	setreg(5, xarg);

	if	(ndef)
	{
		space_body += psub("$1_space(data->$2, $3$5);\n");
		sync_body += psub("$1_sync(data->$2, $3$5);\n");
	}

	if	(iofunc)
	{
		read_body += psub("s += $1_read(io, data->$2, $3$5);\n");
		write_body += psub("s += $1_write(io, data->$2, $3);\n");
	}

	fix = false;
	recl = NULL;
}


void DBData(str Name, str Desc, IO Def, bool glob = true)
{
	DBDataDef x = DBDataDef(Name, Desc, glob);

/*	Typedefinitionen abarbeiten
*/
	str key, type, name, desc;
	bool flag;

	while (desc = psub(fgets(Def)))
	{
		key = xstrcut(desc, "%s");
		type = xstrcut(desc, "%s");
		name = xstrcut(desc, "%s");

		if	(!name)	continue;

		if	(type[0] == '@')
		{
			type = substr(type, 1);
			flag = true;
		}
		else	flag = false;

		if	(key == "std")
			x.std(type, name, desc);
		else if	(key == "ext")
			x.ext(type, name, desc);
		else if	(key == "ptr")
			x.ext(type + " *", name, desc);
		else if	(key == "sub")
			x.sub(type, name, desc, flag);
		else if	(key[0] == '?')
			x.cond(substr(key, 1), type, name, desc);
		else if	(key[0] == '@')
			x.vec(substr(key, 1), type, name, desc, true, flag);
		else if	(key[0] == '*')
			x.vec(substr(key, 1), type, name, desc, false, flag);
	}

/*	Datentype
*/
	x.typedef(glob);	/* Datentype */
	x.makefunc(true);	/* Funktionen */
	x.typeinfo();		/* Datentypeinformation */
}

/*	Ausgabefunktion
*/

void DBPrintFunc (str type, str xdecl, IO def, bool nl = true, bool glob = true)
{
	type = psub(type);
	Function f = Function("int $1_print",
		paste(", ", "io_t *out, $1_t *x, size_t dim", xdecl), type);
	f.line("register int n;");
	f.line();
	f.line("for (n = 0; dim-- > 0; x++)");
	f.beg();
	bool flag = f.pfbody("out", def);

	if	(nl)
	{
		if	(flag)	f.newpar();

		f.line("io_putc('\\n', out);");
		f.line("n++;");
	}

	f.end();
	f.line();
	f.line("return n;");
	f.write(glob);
}

#endif	/* _MS_DBDATA */
