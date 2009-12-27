/*	Zählerdefintion generieren
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#ifndef	_cntdef_ms
#define	_cntdef_ms	1

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
		NULL, NULL, 0,
		DataBase(LabelEntry), DataBase(LabelEntry) };
}

/*	Parameter über interne Variable
*/

virtual inline CntDef CntDef (str name, int n, str def,
	bool glob = true, str pfx = NULL)
{
	name = psub(name);

	if	(def)
		Struct(name, "Klassifikationsvariablen", def, glob);

	fpsub(src, "static $1_t $1_var[$2];\n", name, n);

	Struct("$1_class", "Klassifikationstype", string !
@macro	MDCLASS_VAR	Basisvariablen
$1_t	*ptr		Variablenpointer
!, false);
	str type = psub("$1_class_t", name);

	return (CntDef) { glob, psub(pfx), "const void *data",
		type, type + " *c, const void *data", "c->ptr",
		name + "_t", name + "_var", n,
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
	f.newpar();
	f.body(body);
	f.newpar();
	f.line("return ptr;");
	f.write(glob);
}


void CntDef::ctab(str name, str fmt = NULL, str pext = NULL)
{
	src << "\n/*\tKlassifikationstabelle\n*/\n\n";
	name = psub(name);

	if	(pext)	pext = ", " + psub(pext);

	fprintf(src, "static %s %s[] = {\n", ctype, name);

	for (x in class)
	{
		str cname = fmt ? psub(fmt, x.name) : x.name;
		str fname = paste("_", pfx, x.name);
		fprintf(src, "\t{ %#s, %#s,\n\t", cname, x.desc); 
		fpsub(src, "\t$1_dim, $1_tab, (MdClassify_t) $1_idx", fname);
		src << pext << " },\n";
	}

	src << "};\n\n";
}

void CntDef::setup(str name, str pfx = NULL)
{
	Function f = Function("void $1_setup",
		"xtab_t *count, xtab_t *class, int n", name);

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
	}

	if	(dim(class))
	{
		str fmt = paste(".", pfx, "$1");
		f.line("XAPPEND(class, $1_class, XS_ENTER);", name);
		ctab("$1_class", fmt, var_dim ? "$1_var" : NULL);

		for (int i = 1; i < var_dim; i++)
		{
			f.line("if\t(n <= $2)\treturn;", name, i);
			f.line("XAPPEND(class, $1_class$2, XS_ENTER);");
			ctab("$1_class$2", sprintf("%s.%d", fmt, i + 1),
				var_name + " + $2");
		}
	}

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
	switch(name, desc, expr, "$(key)", def);

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
