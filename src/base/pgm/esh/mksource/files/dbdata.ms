/*
Datenbank - Datenfelder

$Header	<$1>
$Copyright (C) 1998 Erich Frühstück
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

#ifndef	dbdata_ms
#define	dbdata_ms	1

#include <var.ms>
#include <macro.ms>
#include <printfunc.ms>
#include <DataType.ms>

/*	Datenstrukturvariablen
*/

static struct DBDataVar {
	str base;	/* Basisname des Types */
	str type;	/* Variablentype */
	str name;	/* Variablenname */
	str desc;	/* Beschreibungstext */
	str vdim;	/* Vektordimension */
};

static void DBDataVar::typeline(IO io)
{
	if	(name == NULL)	return;

	if	(base)	fprintf(io, "\t%s ", base);
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

static struct DBDataDef {
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
	str s_func;	/* Komponentenfunktionen */
	str s_mtab;	/* Komponententabelle */
	str s_setup;	/* Initialisierungszeilen */
	str s_list;	/* Konverter in Liste */
	str s_create;	/* Konstruktor aus Liste */
	str s_print;	/* Ausgabefunktion als Liste */
};

static DBDataDef DBDataDef(str name, str label, bool glob = true)
{
	name = psub(name);
	label = psub(label);

	if	(!label)	label = psub("Datenstruktur $1", name);
	if	(glob)		fprintf(hdr, "\n/*\t%s\n*/\n", label);

	fprintf(src, "\n/*\t%s\n*/\n\n", label);
	return { name, label, glob, true,
		DataBase(DBDataVar),
		DataBase(str, cmp),
		};
}

static void DBDataDef::use_type (str type)
{
	switch (type)
	{
	case "char":
	case "int":
	case "unsigned":
	case "float":
	case "double":
		break;
	default:
		s_setup += psub("$1_setup();\n", type);
		break;
	}
}

static void DBDataDef::member (str type, str vname, str desc)
{
	use_type(type);
	s_func += psub(string !
static EfiObj *$1_$2 (const EfiObj *base, void *par)
{
	return base ? LvalObj(&Lval_obj, &Type_$3, base,
		&(($1 *) base->data)->$2) : NULL;
}

!, name, vname, type);
	s_mtab += psub("\t{ $2%#s, &Type_$3, $1_$2, NULL, $4%#s },\n",
		name, vname, type, desc);
	s_list += psub(string !
	*ptr = NewObjList(LvalObj(&Lval_ptr, &Type_$1, &data->$2));
	ptr = &(*ptr)->next;
!, type, vname);
	s_print += psub(string !
n += PrintData(io, &Type_$1, &data->$2);
n += io_puts(", ", io);
!, type, vname);
	s_create += psub(string !

if	(list)
{
	Obj2Data(RefObj(list->obj), &Type_$1, &data->$2);
	list = list->next;
}
else	CleanData(&Type_$1, &data->$2);
!, type, vname);
}

/*	Datentype generieren
*/

static void DBDataDef::typedef(bool flag = true)
{
	IO io = flag ? hdr : top;
	int n = dim(var);
	int i;

	Function f;
	DataTypeHeader(flag);
	io << "\ntypedef struct {\n";

	for (i = 0; i < n; i++)
		var[i].typeline(io);

	fprintf(io, "} %s;\n\n", name);

	Macro(io, "$1_recl", NULL, (fix ? "($2)" : "0"), name, recl);
	Macro(io, "$1_size", NULL, "sizeof($1)", name, recl);

	if	(!read_body)
		Macro(io, "$1_read", "io, data, dim",
			"io_dbread(io, data, $1_recl, $1_size, dim)", name);

	if	(!write_body)
		Macro(io, "$1_write", "io, data, dim",
			"io_dbwrite(io, data, $1_recl, $1_size, dim)", name);

	if	(!clear_body)
		Macro(io, "$1_clear", "data, dim",
			"memset(data, 0, (dim) * $1_size)", name);

	f = Function("size_t Type_$1_read",
		"const EfiType *type, void *data, IO *io", name);

	if	(argdecl)
	{
		f.body(string !
fprintf(stderr, "Datentyp %s kann nicht gelesen werden.\n", type->name);
$1_clear(data, 1);
return 0;
!, name);
	}
	else	f.body("return $1_read(io, data, 1);", name);
	
	f.write(false);

	LocalFunction("size_t Type_$1_write",
		"const EfiType *type, const void *data, IO *io",
		"return $1_write(io, data, 1);", name);
	
	LocalFunction("void Type_$1_clean",
		"const EfiType *type, void *data",
		"$1_clear(data, 1);", name);

	f = Function("void Type_$1_copy",
		"const EfiType *type, void *tg, const void *src",
		name);

	if	(argdecl)
	{
		f.body(string !
fprintf(stderr, "Datentyp %s kann nicht kopiert werden.\n", type->name);
$1_clear(tg, 1);
!, name);
	}
	else
	{
		f.body(string !
IO *io = io_tmpbuf(0);
$1_write(io, (void *) src, 1);
io_rewind(io);
$1_read(io, tg, 1);
io_close(io);
!, name);
	}

	f.write(true);

	if	(flag)
	{
		fpsub(hdr, "extern EfiType Type_$1;\n", name);
	}
	else	src << "static ";

	fpsub(src, string !
EfiType Type_$1 = COMPLEX_TYPE("$1", "$1", sizeof($1), 0,
	Type_$1_read, Type_$1_write, Type_$1_print,
	NULL, NULL, NULL, Type_$1_clean, Type_$1_copy);
!, name);

	io << "\n";
	src << s_func;

	Function setup = Function("void $1_setup", NULL, name);
	setup.line("static int setup_done = 0;");
	setup.line();
	setup.line("if	(setup_done)	return;");
	setup.line();
	setup.line("setup_done = 1;");
	setup.body(s_setup);
	setup.line("AddType(&Type_$1);", name);

	if	(s_mtab)
	{
		fprintf(src, "static EfiMember %s_mtab[] = {\n%s};\n\n",
			name, s_mtab);
		setup.line("AddEfiMember(Type_$1.vtab, \
$1_mtab, tabsize($1_mtab));", name);
	}

	if	(s_list)
	{
		Function to_list = Function("void $1_to_list", 
			"EfiFunc *func, void *rval, void **arg", name);
		to_list.line("$1 *data = arg[0];", name);
		to_list.line("EfiObjList **ptr = rval;");
		to_list.body(s_list);
		to_list.write(false);
		setup.line("SetFunc(0, &Type_list, \"$1()\", $1_to_list);",
			name);
	}

	Function xprint = Function("int Type_$1_print", 
		"const EfiType *type, const void *opaque, IO *io",
		name);
	xprint.line("const $1 *data = opaque;", name);
	xprint.line("int n = io_puts(\"{ \", io);", name);
	xprint.body(s_print);
	xprint.line("n += io_puts(\"}\", io);");
	xprint.line("return n;");
	xprint.write(false);

	if	(s_create)
	{
		Function f_create = Function("void $1_create", 
			"EfiFunc *func, void *rval, void **arg", name);
		f_create.line("$1 *data = rval;", name);
		f_create.line("EfiObjList *list = Val_list(arg[0]);");
		f_create.body(s_create);
		f_create.write(false);
		setup.line("SetFunc(0, &Type_$1, \"$1(List_t)\", $1_create);",
			name);
	}

	setup.write(flag);
}

/*	Datentypefunktionen generieren
*/

static void DBDataDef::makefunc(bool glob = true)
{
	Function f;

	str arg_read = "IO *io, $1 *data, size_t n";
	str arg_write = "IO *io, const $1 *data, size_t n";
	str arg_reorg = "$1 *data, size_t n";
	str var_reorg = paste(", ", "data, n", arglist);

	str space_set;
	str sync_copy;

	for (x in var)
	{
		if	(!x.vdim)	continue;

		List_t args = { name, x.name, "data->" + x.vdim };

		if	(slist[x.name])
		{
			space_set += psub("$3 = $2_buf - data->$2;\n", args);
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
!, args);
		}
	}

/*	Eingabefunktion
*/
	if	(read_body)
	{
		f = Function("size_t $1_read",
			paste(", ", arg_read, argdecl), name);
		f.body(varlist + "register size_t s;");
		f.line();
		f.line("for (s = 0; n-- > 0; data++)");
		f.beg();
		f.body(read_body, name);
		f.end();
		f.line();
		f.line("return s;");
		f.write(glob);
	}

/*	Ausgabefunktion
*/
	if	(write_body)
	{
		f = Function("size_t $1_write", arg_write, name);
		f.line("register size_t s = 0;");
		f.line();
		f.line("for (s = 0; n-- > 0; data++)");
		f.beg();
		f.body(write_body, name);
		f.end();
		f.line("return s;");
		f.write(glob);
	}

/*	Aufräumen
*/
	if	(clear_body)
	{
		f = Function("void $1_clear", "$1 *data, size_t n", name);
		f.body("while (n-- > 0)");
		f.beg();
		f.body(clear_body);
		f.line("memset(data, 0, sizeof($1));", name);
		f.body("data++;");
		f.end();
		f.write(glob);
	}

/*	Datenreorganisation
*/
	if	(alloc_body)
	{
		f = Function("void $1_alloc", arg_reorg, name);
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
		f = Function("void $1_space",
			paste(", ", arg_reorg, argdecl), name);
		f.body(varlist, name);
		f.line()
		f.line("for (; n-- > 0; data++)");
		f.beg();
		f.body(space_body, name);
		f.body(space_set, name);
		f.end();
		f.write(glob);
	}
	else	Macro(src, "$1_space", "data, dim", NULL, name);

	if	(sync_copy)
		syncvar += "register size_t i;\n";
		
	if	(sync_body)
	{
		f = Function("void $1_sync",
			paste(", ", arg_reorg, argdecl), name);
		f.body(varlist + syncvar, name);
		f.line("for (; n-- > 0; data++)");
		f.beg();
		f.body(sync_head, name);
		f.body(sync_copy, name);
		f.body(sync_body, name);
		f.body(sync_foot, name);
		f.end();
		f.write(glob);
	}
	else	Macro(src, "$1_sync", "data, dim", NULL, name);
}


/*	Datentypebeschreibungstabelle generieren
*/

static void DBDataDef::typeinfo()
{
	fpsub(info, "@$1:$2\n", name, label);

	for (x in var)
		fprintf(info, "%s %s\t%s\n", x.type, x.name, x.desc)

	info << '\n';
}


/*	Standardvariable
*/

static void DBDataDef::std & (str type, str vname, str desc)
{
	var += { NULL, type, vname, desc };
	str size = sprintf("sizeof(%s)", type);
	read_body += sprintf("s += io_dbread(io, &data->%s, %s, %s, 1);\n",
		vname, size, size);
	write_body += sprintf("s += io_dbwrite(io, &data->%s, %s, %s, 1);\n",
		vname, size, size);

	switch (type)
	{
	case "double":
		s_print += sprintf("n += io_printf(io, %#s, &data->%s);",
			"%.18g, ", vname);
		break;
	}

	if	(fix)
		recl = paste(" + ", recl, size);
}


static void DBDataDef::ext & (str type, str vname, str desc)
	var += { NULL, (type == "." ? NULL : type), vname, desc };

/*	Zeichenketten
*/

static void DBDataDef::str & (str size, str name, str desc)
{
	var += { NULL, "char", psub("$1[$2]", name, size), desc };
	read_body += sprintf("s += io_read(io, &data->%s, %s);\n",
		name, size);
	write_body += sprintf("s += io_write(io, &data->%s, %s);\n",
		name, size);

	if	(fix)
		recl = paste(" + ", recl, size);
}

/*	Teildatentype mit Standardnamenskonvention
*/

static void DBDataDef::sub & (str base, str ndef, str desc, bool flag)
{
	str vname = xstrcut(ndef, ",");

	member(base, vname, desc);
	var += { base, base, vname, desc };

	str xarg;

	for (x in split(ndef, ","))
	{
		slist += x;
		xarg += sprintf(", &%s_buf", x);
	}

	List_t args = { base, vname, "1", xarg };

	if	(ndef)
	{
		space_body += psub("$1_space(&data->$2, $3$4);\n", args);
		sync_body += psub("$1_sync(&data->$2, $3$4);\n", args);
	}

	read_body += psub("s += $1_read(io, &data->$2, $3$4);\n", args);
	write_body += psub("s += $1_write(io, &data->$2, 1);\n", args);

	if	(flag)
	{
		clear_body += psub("$1_clear(&data->$2, 1);\n", args);
		fix = false;
	}
	else if	(fix)
	{
		recl = paste(" + ", recl, psub("$1_recl", args));
	}
}

static void DBDataDef::cond & (str key, str base, str vname, str desc)
{
	member(base, vname, desc);
	var += { base, base, vname, desc };

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

	List_t args = { base, vname };
	read_body += beg_key;
	read_body += psub("\ts += $1_read(io, &data->$2, 1);\n", args);
	read_body += else_key;
	read_body += psub("\t$1_clear(&data->$2, 1);\n", args);
	read_body += end_key;
	write_body += beg_key;
	write_body += psub("\ts += $1_write(io, &data->$2, 1);\n", args);
	write_body += end_key;
	fix = false;
	recl = NULL;
}


/*	Vektor
*/

static void DBDataDef::vec & (str key, str base, str ndef, str desc,
	bool iofunc, bool flag)
{
	str vecdim = strcut(key, ",");

	if	(vecdim == NULL)	continue;

	str vname = xstrcut(ndef, ",");

	use_type(base);
	var += { base, base, vname, desc, vecdim };
	Function f;

	ArgList arg = ArgList(name, base, vname, "data->" + vecdim, NULL, NULL);
	varlist += psub("$1 *$2_buf;\n", arg);

	if	(key)
	{
		str vecsize = vname + "_size";
		var += { NULL, "size_t", vecsize, NULL };
		arg.set(4, "data->" + vecsize);
		arg.set(5, key);

		str clear;
		str realloc;

		if	(flag)
			clear = "$1_clear(data->$2, $4);\n";

		clear += "lfree(data->$2);\n";

		realloc = "$4 = sizealign($3, $5);";
		realloc += "\ndata->$2 = lmalloc($4 * sizeof($1));";

		if	(flag)
			realloc += "\nmemset(data->$2, 0, $4 * sizeof($1));";

		clear = psub(clear, arg);
		realloc = psub(realloc, arg);

		f = Function("void $1_$2_realloc", "$1 *data", name, vname);
		f.fline("if\t(%s > %s)", arg[3], arg[4]);
		f.beg();
		f.body(clear);
		f.body(realloc);
		f.end();
		f.write(false);

		clear_body += clear;
		alloc_body += psub("$0_$2_realloc(data);\n", arg);
		read_body += psub("$0_$2_realloc(data);\n", arg);
		syncvar += psub("$1 *$2_save;\n", arg);

		sync_head += psub("\n$2_save = $4 ? data->$2 : NULL;\n", arg);
		sync_head += psub("$4 = sizealign($3, $5);\n", arg);
		sync_head += psub("$2_buf = lmalloc($4 * sizeof($1));\n", arg);
		sync_foot += psub("lfree($2_save);\n", arg);
	}
	else
	{
		argdecl = paste(", ", argdecl, psub("$1 **$2_ptr", arg));
		arglist = paste(", ", arglist, psub("$2_ptr", arg));

		space_body += psub("*$2_ptr += $3;\n", arg);
		read_body += psub("data->$2 = *$2_ptr;\n*$2_ptr += $3;\n", arg);
		sync_head += psub("\n$2_buf = *$2_ptr;\n*$2_ptr += $3;\n", arg);
	}

	read_body += psub("$2_buf = data->$2;\n", arg);
	space_body += psub("$2_buf = data->$2;\n", arg);
	sync_body += psub("data->$2 = $2_buf;\n", arg);

	str xarg = NULL;

	for (x in split(ndef, ","))
	{
		slist += x;
		xarg += sprintf(", &%s_buf", x);
	}

	arg.set(5, xarg);

	if	(ndef)
	{
		space_body += psub("$1_space(data->$2, $3$5);\n", arg);
		sync_body += psub("$1_sync(data->$2, $3$5);\n", arg);
	}

	if	(iofunc)
	{
		read_body += psub("s += $1_read(io, data->$2, $3$5);\n", arg);
		write_body += psub("s += $1_write(io, data->$2, $3);\n", arg);
	}

	s_func += psub(string !
static EfiObj *$0_$2 (const EfiObj *base, void *par)
!, arg);
	s_func += "{\n\tif\t(base)\n\t{\n";
	s_func += psub("\t\t$0 *data = ($0 *) base->data;\n", arg);

	if	(key)
		s_func += psub("\t\t$0_$2_realloc(data);\n", arg);

	s_func += psub(string !
		return EfiVecObj(&Type_$1, data->$2, $3);
!, arg);
	s_func += "\t}\n\treturn NULL;\n}\n\n";
	s_mtab += psub(string !
	{ $2%#s, &Type_$1, $0_$2, NULL, $(desc:%#s) },
!, arg);
	if	(key)
	{
		s_list += psub("\t\t$0_$2_realloc(data);\n", arg);
		s_create += psub("\t$0_$2_realloc(data);\n", arg);
	}
		
	s_list += psub(string !
	*ptr = NewObjList(EfiVecObj(&Type_$1, data->$2, $3));
	ptr = &(*ptr)->next;
!, arg);
	s_create += psub(string !

	if	(list)
	{
		Obj2VecData(RefObj(list->obj), &Type_$1, $3, data->$2);
		list = list->next;
	}
	else	CleanVecData(&Type_$1, $3, data->$2);
!, arg);
	fix = false;
	recl = NULL;
}

/*
Die Funktion |$1| generiert eine Datenstruktur mit Namen |Name|
aus der Eingabestruktur |Def| mit Definitionszeilen.
Das Argument |Desc| dient als Beschreibungstext zur Erläuterung der
Sourcen. Das Flag <glob> bestimmt, ob der Datentyp global (im Header) oder
local (im Sourcefile) definiert wird.

Zu jedem Datentyp werden Hilfsfunktionen zum Löschen,
zur binären Eingabe und zur binären Ausgabe generiert.

Eine Definitionszeile hat den folgenden Aufbau:
	<key> <type> <name> <desc>

wobei <key> ein Kennwort, <type> ein Datentyp, <name> ein
Variablenname und <desc> einen Beschreibungstext für die Variable
darstellt.

Folgende Kennungen sind definiert:
\ttitem	std
	Definition einer gewöhnlichen Variable vom Type <type>
\ttitem	ext
	Definition einer externen Variablen, sie wird bei der binären
	Ein- und Ausgabe nicht berücksichtigt.
\ttitem	ptr
	Definition eines externen Pointers.
\ttitem	sub
	Definition einer Variablen mit einem Datentyp, der Funktionen
	zur binären Ein- und Ausgabe unterstützt (typischerweise ein
	mit |Bitfield| oder |DBData| eingerichtetet Datentyp).
\ttitem	?<cond>
	Wie <sub>, jedoch werden Ein- und Ausgabefunktionen in
	Abhängigkeit von der Bedingung <cond> (eine andere Komponente
	des Datentyps) aufgerufen. Ist die Bedingung beim Lesen
	falsch, wird der Datenwert auf 0 gesetzt.
\ttitem	\@<var>,<bsize>
	Definition einer Vektorkomponente, wobei <var> die aktuelle
	Dimension des Vektors angibt und <bsize> die Blockgröße zur
	Vergrößerung des Speicherfeldes für den Vektor bestimmt.
\ttitem	*<var>
	Wie |@|, jedoch wird der Speicherbereich der Variablen extern
	zur Verfügung gestellt. Wird bei mehrdimensionalen
	Datenstrukturen verwendet.

Falls bei einem Datentyp ein |@| vorangestellt ist, wird beim
Löschen eines Datensatzes auch für diese Komponente eine eigene
Löschfunktion (muss definiert sein) aufgerufen.

Bei einer Vektordefinition können dem Namen zusätzliche Komponenten
muit ',' angehängt werden. Dabei handelt es sich um eine andere
Vektorkomponente, die als externer Speicherbereich für Teilvektoren
benötigt werden.
*/

void DBData (str Name, str Desc, IO Def, bool glob = true)
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
		else if	(key == "str")
			x.str(type, name, desc);
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

/*
Die Funktion |$1| generiert eine Ausgabefunktion für Datenvektoren
mithilfe von |Function::pfbody| (Vergleiche dazu \mref{printfunc(7)}).
In den Definitionszeilen verweist <x> auf den aktuellen Datenpointer.
Normalerweise akzeptiert die generierte Ausgabefunktion eine
Ausgabestruktur, den Pointer auf den Datenvektor und die Dimension
des Datenvektors. Mit <xdecl> können zusätzliche Argumente übergeben
werden.
*/

void DBPrintFunc (str type, str xdecl, IO def, bool nl = true, bool glob = true)
{
	type = psub(type);
	Function f = Function("int $1_print",
		paste(", ", "IO *out, $1 *x, size_t dim", xdecl), type);
	f.line("register int n;");
	f.line();
	f.line("for (n = 0; dim-- > 0; x++)");
	f.beg();
	bool flag = f.pfbody(def, "out", NULL);

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

/*
$Example
Das folgende Beispiel zeigt die Definition einer mehrfach
verschachtelten Datenstruktur

\ms_example
config hdr src

#include <dbdata.ms>

DBData("T1", "Datentyp 1", string !
std	int	val	Datenwert
!);

DBData("T2", "Datentyp 2", string !
std	int	dim	Dimension des Teilvektors
*dim	T1	ptr	Teilvektor
!);

DBData("TX", "Komplexer Datentyp", string !
std	int	n1	Dimension des 1. Datenvektors
std	int	n2	Dimension des 2. Datenvektors
@n1,64	T1	p1	1. Datenvektor
@n2,16	T2	p2,p1	2. Datenvektor
!);
\end

$SeeAlso
\mref{mksource(1)}, \mref{printfunc(7)}.
*/

#endif	/* dbdata.ms */
