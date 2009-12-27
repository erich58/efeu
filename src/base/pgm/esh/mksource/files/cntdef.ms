/*
Zählerdefintion generieren

$Copyright (C) 2000 Erich Frühstück
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

#ifndef	cntdef_ms
#define	cntdef_ms	1

#include <struct.ms>
#include <function.ms>
#include <index.ms>
#include <cntobj.ms>

struct CntDef {
	bool glob;	/* Flag für globale Definition */
	str pfx;	/* Namensprefix */
	str cntarg;	/* Argumentliste der Zählerfunktionen */
	str ctype;	/* Type der Klassendefinitionen */
	str idxarg;	/* Argumentliste der Klassenfunktionen */
	str ptr;	/* Pointer mit Klassifikationsvariablen */
	str var_type;	/* Variablentype */
	str var_name;	/* Variablenname */
	int var_dim;	/* Dimension des Variablenvektors */
	DataBase ini;	/* Datenbank mit Initialisierungsfunktionen */
	DataBase cnt;	/* Datenbank mit Zählern */
	DataBase class;	/* Datenbank mit Klassifikationen */
};


/*	Parameter über Datenpointer
*/

virtual inline CntDef CntDef (str type = NULL, bool glob = true,
	str pfx = NULL)
{
	str arg = (type ? psub(type) : "const void") + " *data";
	return (CntDef) { glob, psub(pfx), arg,
		"MdClass_t", "MdClass_t *c, " + arg, "data",
		NULL, NULL, 0, DataBase(str),
		DataBase(LabelEntry), DataBase(LabelEntry) };
}

/*	Parameter über interne Variable
*/

virtual inline CntDef CntDef (str name, int n, str def,
	str vname = NULL, bool glob = true, str pfx = NULL)
{
	name = psub(name);

	if	(def)
		Struct(name, "Klassifikationsvariablen", def, glob);

	if	(!vname)
	{
		src << "static ";
		vname = name + "_var";
	}
	else if	(!glob)
	{
		src << "static ";
	}
	else	fpsub(hdr, "extern $1_t $2[$3];\n", name, vname, n);

	fpsub(src, "$1_t $2[$3];\n", name, vname, n);
	Struct("$1_class", "Klassifikationstype", string !
@macro	MDCLASS_VAR	Basisvariablen
$1_t	*ptr		Variablenpointer
!, false);
	str type = psub("$1_class_t", name);

	return (CntDef) { glob, psub(pfx), "const void *data",
		type, type + " *c, const void *data", "c->ptr",
		name + "_t", vname, n, DataBase(str),
		DataBase(LabelEntry), DataBase(LabelEntry) };
}


/*	Initialisierungsfunktion für Variablenvektor
*/

void CntDef::init(str name, str args = NULL, str body = NULL)
{
	Function f = Function("$1 *" + name, paste(", ", "int n", args),
		var_type, var_name, var_dim);
	f.line("static $1 buf, *ptr;");
	f.newpar();
	f.line("ptr = n < $3 ? $2 + n : &buf;");
	f.line("memset(ptr, 0, sizeof($1));");

	if	(body)
	{
		f.line();
		f.beg();
		f.body(body);
		f.end();
		f.line();
	}

	f.line("return ptr;");
	f.write(glob);
}


void CntDef::ctab(str name, str fmt = NULL, str pext = NULL, str xdesc = NULL)
{
	src << "\n/*\tKlassifikationstabelle\n*/\n\n";
	name = psub(name);

	if	(pext)	pext = ", " + psub(pext);

	fprintf(src, "static %s %s[] = {\n", ctype, name);

	for (x in class)
	{
		str cname = fmt ? psub(fmt, x.name) : x.name;
		str fname = paste("_", pfx, x.name);
		fprintf(src, "\t{ %#s, %#s,\n\t", cname,
			paste(", ", x.desc, xdesc)); 
		fpsub(src, "\t$1_dim, $1_tab, (MdClassify_t) $1_idx", fname);
		src << pext << " },\n";
	}

	src << "};\n\n";
}

void CntDef::setup(str name, str lblpfx = NULL, ...)
{
	Function add = Function("void $1_addclass",
		"xtab_t *class, int n", name);
	Function f = Function("void $1_setup",
		"xtab_t *count, xtab_t *class, int n", name);

	for (x in ini)
		f.line(x);

	if	(dim(cnt))
	{
		src << "\n/*\tZählertabelle\n*/\n\n";
		fprintf(src, "static MdCount_t %s_cnt[] = {\n", name);

		for (x in cnt)
		{
			fprintf(src, "\t{ %#s, ", x.name); 
			fprintf(src, "CT_%s, ", x.name); 
			fprintf(src, "%#s,\n\t\t", x.desc); 
			fprintf(src, "(MdSet_t) set_%s, ", x.name);
			fprintf(src, "(MdInit_t) ini_%s, ", x.name);
			fprintf(src, "(MdAdd_t) add_%s },\n", x.name);
		}

		src << "};\n\n";
		f.line("XAPPEND(count, $1_cnt, XS_ENTER);", name);
		cnt.delete(0, dim(cnt));
	}

	int n = dim(class);

	if	(n)
	{
		str xdesc = va_list.pop();
		str ext = strcut(xdesc, "%s");
		str fmt = paste(".", lblpfx, "$1");
		str lbl = paste(".", fmt, ext == "." ? NULL : ext);
		str cname = name + "_class";

		f.line("$1_addclass(class, 0);", name);
		psub(NULL, cname, 0);
		ctab("$1$2", lbl, var_dim ? var_name : NULL, xdesc);

		if	(var_dim > 1)
		{
			f.line();
			f.line("while\t(n-- > 1)");
			f.line("\t$1_addclass(class, n);", name);
		}

		for (int i = 1; i < var_dim; i++)
		{
			xdesc = va_list.pop();
			ext = strcut(xdesc, "%s");

			if	(ext && ext != ".")
				lbl = paste(".", fmt, ext);
			else	lbl = sprintf("%s.%d", fmt, i + 1);

			psub(NULL, cname, i);
			ctab("$1$2", lbl, var_name + " + $2", xdesc);
		}

		fprintf(src, "#define %s_size %d\n\n", cname, n);
		fprintf(src, "static %s *%s[] = {\n", ctype, cname);
		fprintf(src, "\t%s0,\n", cname);

		for (i = 1; i < var_dim; i++)
			fprintf(src, "\t%s%d,\n", cname, i);

		src << "};\n\n";

		add.fline("if\t(n < %d)", var_dim ? var_dim : 1);
		add.line("\txappend(class, $1[n], $1_size,", cname);
		add.line("\t\tsizeof($1), XS_ENTER);", ctype);

		class.delete(0, dim(class));
	}
	else	add.line(";");

	add.write(glob);
	f.write(glob);
}

void CntDef::add_cnt(str name, str desc)
{
	fprintf(src, "\n/*\t%s\n*/\n\n",
		psub(desc ? desc : "Zählobjekt $(name)"));
	cnt += { name, desc };
}

void CntDef::add_class(str name, str desc)
{
	fprintf(src, "\n/*\t%s\n*/\n\n",
		psub(desc ? desc : "Klassifikation $(name)"));
	class += { name, desc };
}

void CntDef::use(...)
{
	for (x in va_list)
	{
		str n = x;
		str name = strcut(n, "[");

		if	(n)
		{
			ini += psub("$1_addclass(class, $2);", name, (int) n);
		}
		else	ini += psub("$1_setup(count, class, n);", name);
	}
}

/*	Zählobjekt definieren
*/

void CntDef::cntobj(str name, str desc, Type_t type, IO def)
{
	add_cnt(name, desc);
	str ptr = var_dim ? var_name : "data";
	CntObj(name, type, def, cntarg);
}


/*	Klassifikationsfunktion generieren
*/

Function CntDef::func(str name, ...)
	Function("int " + name + "_idx", idxarg, va_list)

void CntDef::stdfunc(str name, str fdef, ...)
{
	Function f = func(name, va_list);
	f.body(fdef);
	f.write(false);
}

void CntDef::classfunc(str name, str desc, str fdef, IO def)
{
	add_class(name, desc);
	name = paste("_", pfx, name);
	stdfunc(name, fdef);
	Label lbl = Label(name);
	lbl.load(def);
	lbl.tab(false);
}

void CntDef::simple(str name, str desc, str expr, int n,
	str fmt = NULL, str label = NULL)
{
	add_class(name, desc);
	name = paste("_", pfx, name);
	stdfunc(name, "return " + expr + ";");
	Label lbl = Label(name);
	lbl.range(fmt, label, 0, n - 1);
	lbl.tab(false);
}

void CntDef::limit(str name, str desc, str expr, int n,
	str fmt = NULL, str label = NULL)
{
	add_class(name, desc);
	name = paste("_", pfx, name);
	stdfunc(name, "register int x = " + expr + \
		sprintf(";\nreturn min(x, %d);", n - 1));
	Label lbl = Label(name);
	lbl.range(fmt, label, 0, n - 1);
	lbl.tab(false);
}

void CntDef::range(str name, str desc, str expr, int first, int last,
	str fmt = NULL, str label = NULL)
{
	add_class(name, desc);
	name = paste("_", pfx, name);
	stdfunc(name, string !
register int x = ($1);

if	(x <= $2)	return 0;
if	(x <= $3)	return x - $2;
return $3 - $2;
!, psub(expr), first, last);
	Label lbl = Label(name);
	lbl.range(fmt, label, first, last);
	lbl.tab(false);
}

void CntDef::flag(str name, str desc, str expr, str fmt = NULL)
{
	add_class(name, desc);
	name = paste("_", pfx, name);
	stdfunc(name, "return (" + expr + ") ? 0 : 1;");
	Label lbl = Label(name);
	lbl.add((fmt ? fmt : name), desc);
	lbl.tab(false);
}

virtual void CntDef::switch(str name, str desc, str expr, str fmt, IO def)
{
	add_class(name, desc);
	IndexFunc f = IndexFunc(paste("_", pfx, name), idxarg);

	if	(!f.switch(expr, fmt, def))
		f.func.fline("return %d;", dim(f.lbl));
		
	f.write(false);
}

virtual void CntDef::switch(str name, str desc, str expr, IO def)
	this.switch(name, desc, expr, "$(key)", def);

virtual void CntDef::test(str name, str desc, str init, str fmt, IO def)
{
	add_class(name, desc);
	IndexFunc f = IndexFunc(paste("_", pfx, name), idxarg);
	f.func.body(init);

	if	(!f.test(fmt, def))
		f.func.fline("return %d;", dim(f.lbl));
		
	f.write(false);
}

virtual void CntDef::test(str name, str desc, str init, IO def)
	test(name, desc, init, "$(key)", def);


void CntDef::complex(str name, str desc, IO def)
{
	add_class(name, desc);
	IndexFunc x = IndexFunc(paste("_", pfx, name), idxarg);
	x.body(def);
	x.write(false);
}

#endif	/* cntdef.ms */
