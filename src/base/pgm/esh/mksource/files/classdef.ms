/*	Klassifikationsfunktionen generieren
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_MS_CLASSDEF
#define	_MS_CLASSDEF	1

#include <function.ms>

struct ClassDefEntry {
	str name;	/* Name */
	str desc;	/* Beschreibungstext */
	str ext;	/* Zusatzvariablen */
};

struct ClassDefLabel {
	str name;	/* Name */
	str desc;	/* Beschreibungstext */
};

void ClassDefLabel::print(IO io)
	fprintf(io, "\\Entry{%s}{%s}\n", name, desc);

struct ClassDef {
	str type;	/* Type der Klassendefinition */
	str arglist;	/* Argumentliste der Klassenfunktionen */
	str ptr;	/* Pointer mit Klassifikationsvariablen */
	DataBase db;	/* Datenbank mit Einträgen */
};


inline void ClassType(IO _io, str _name, ...)
{
	fprintf(_io, "typedef struct {\n\tMDCLASS_VAR;\n");

	for (_x in va_list)
		fprintf(_io, "\t%s;\n", psub(_x));

	fprintf(_io, "} %s;\n\n", psub(_name));
}

inline ClassDef ClassDef (str args = NULL, str type = NULL, str ptr = NULL)
{
	type = type ? psub(type) : "MdClass_t *class";
	args = args ? psub(args) : "const void *data";
	ptr = ptr ? psub(ptr) : "ptr";
	return { split(type).obj, paste(", ", type, args), ptr,
		DataBase(ClassDefEntry) };
}

void ClassDef::tab(str name, bool glob = false, str fmt = NULL, str pext = NULL)
{
	if	(!type)	return;

	src << "\n/*\tKlassifikationstabelle\n*/\n\n";
	name = psub(name);

	if	(pext)	pext = ", " + psub(pext);

	fprintf((glob ? hdr : src), "\n#define\t%s_dim\t%d\n\n", name, dim(db));

	if	(glob)
	{
		fprintf(hdr, "extern %s %s_tab[%s_dim];\n", type, name, name);
	}
	else	fprintf(src, "static ");

	fprintf(src, "%s %s_tab[%s_dim] = {\n", type, name, name);

	for (x in db)
	{
		str cname = fmt ? psub(fmt, x.name) : x.name;
		fprintf(src, "\t{ %#s, %#s,\n\t", cname, x.desc); 
		fpsub(src, "\t$1_dim, $1_tab, (MdClassify_t) $1_idx", x.name);
		src << pext;
		src << " },\n";
	}

	src << "};\n\n";
}

#if	0
void ClassDef::setup(str name, str ext = NULL, str pext = NULL)
{
	tab(name);
	Function f = Function("void $1", "xtab_t *tab", name);
	f.line("XAPPEND(tab, $1_tab, XS_ENTER);");
	f.write(true);
}
#endif

void ClassDef::setup(str name, bool glob = true)
{
	name = psub(name);
	tab(name);
	Function f = Function("void $1", "xtab_t *tab", name);
	f.line("XAPPEND(tab, $1_tab, XS_ENTER);");
	f.write(glob);
}

void ClassDef::addentry(str name, str desc, str cdim = NULL,
	str lbl = NULL, str cf = NULL)
{
	fprintf(src, "\n/*\t%s\n*/\n\n", desc);

	if	(cdim)	fprintf(src, "#define\t%s_dim\t%s\n", name, psub(cdim));
	if	(lbl)	fprintf(src, "#define\t%s_tab\t%s\n", name, psub(lbl));
	if	(cf)	fprintf(src, "#define\t%s_idx\t%s\n", name, psub(cf));

	db += { name, desc, NULL };
}

/*	Klassifikationsbeschreibung generieren
*/

virtual void ClassDef::mklabel(str name, DataBase label)
{
	int n = dim(label);

	fprintf(src, "#define\t%s_dim\t%d\n\n", name, n);
	fprintf(src, "static Label_t %s_tab[%d] = {\n", name, n);

	for (x in label)
		fprintf(src, "\t{ %#s, %#s },\n", x.name, x.desc);

	src << "};\n\n";
}

virtual void ClassDef::mklabel(str name, IO def)
{
	DataBase lbl = DataBase(ClassDefLabel);
	str a, b; 

	while (getline(def, a, b))
		lbl += { a, b };

	mklabel(name, lbl);
}

virtual void ClassDef::mklabel(str name, str fmt, str label,
	int first, int last)
{
	DataBase lbl = DataBase(ClassDefLabel);

	for (int n = first; n <= last; n++)
		lbl += { psub(fmt), psub(label) };

	mklabel(name, lbl);
}

/*	Klassifikationsfunktion generieren
*/

Function ClassDef::func(str name, ...)
	Function("int " + name + "_idx", arglist, va_list)

void ClassDef::stdfunc(str name, str fdef, ...)
{
	Function f = func(name, va_list);
	f.body(fdef);
	f.write(false);
}

void ClassDef::simple(str name, str desc, str expr, int n,
	str fmt = NULL, str label = NULL)
{
	addentry(name, desc);
	stdfunc(name, "return " + expr + ";");
	mklabel(name, fmt, label, 0, n - 1);
}

void ClassDef::classfunc(str name, str desc, str fdef, IO def)
{
	addentry(name, desc);
	stdfunc(name, fdef);
	mklabel(name, def);
}

void ClassDef::limit(str name, str desc, str expr, int n,
	str fmt = NULL, str label = NULL)
{
	addentry(name, desc);
	stdfunc(name, "register int x = " + expr + \
		sprintf(";\nreturn min(x, %d);", n - 1));
	mklabel(name, fmt, label, 0, n - 1);
}

void ClassDef::range(str name, str desc, str expr, int first, int last,
	str fmt = NULL, str label = NULL)
{
	addentry(name, desc);
	stdfunc(name, string !
register int x = ($1);

if	(x <= $2)	return 0;
if	(x <= $3)	return x - $2;
return $3 - $2;
!, psub(expr), first, last);
	mklabel(name, fmt, label, first, last);
}

void ClassDef::flag(str name, str desc, str expr, str fmt = NULL)
{
	DataBase label = DataBase(ClassDefLabel);
	addentry(name, desc);
	stdfunc(name, "return (" + expr + ") ? 0 : 1;");
	label += { (fmt ? fmt : name), desc };
	mklabel(name, label);
}

virtual void ClassDef::switch(str name, str desc, str expr, str fmt, IO def)
{
	DataBase lbl = DataBase(ClassDefLabel);
	str key, idx, label;
	int n;

	addentry(name, desc);
	Function f = func(name);
	f.line("switch (" + expr + ")");
	f.beg();
	n = 0;

	while (getline(def, key, idx, label))
	{
		str cmd = NULL;
		idx = psub(idx);
		label = psub(label);

		if	(strlen(idx))
		{
			cmd = sprintf("return %d; /* %s */", n++, idx);
			lbl += { idx, label };
		}

		if	(key != "*")
		{
			key = psub(key);
			f.case(psub(fmt));
		}
		else	f.case(NULL);

		f.line(cmd);
	}

	f.end();
	f.line();
	f.fline("return %d;", n);
	f.write(false);
	mklabel(name, lbl);
}

virtual void ClassDef::switch(str name, str desc, str expr, IO def)
	switch(name, desc, expr, "$(key)", def);

virtual void ClassDef::test(str name, str desc, str init, str fmt, IO def)
{
	DataBase lbl = DataBase(ClassDefLabel);
	str key, idx, label;
	bool need_return = true;

	addentry(name, desc);
	Function f = func(name);
	f.body(init);
	str last = NULL;
	int n = 0;

	while (getline(def, key, idx, label))
	{
		str test = NULL;

		if	(key != "*")
		{
			key = psub(key);
			test = "if\t( " + psub(fmt) + " )\n\t\t";
		}
		else	need_return = false;

		if	(strlen(idx))
		{
			idx = psub(idx);
			f.fline("%sreturn %d; /* %s */", test, n, idx);
			lbl += { idx, psub(label) }
			last = key;
			n++;
		}
		else	f.fline("%sreturn %d;", test, n);
	}

	if	(need_return)
		f.fline("return %d;", n);

	f.write(false);
	mklabel(name, lbl);
}

virtual void ClassDef::test(str name, str desc, str init, IO def)
	test(name, desc, init, "$(key)", def);


/*	Komplexe Klassifikationen
*/

struct ComplexClass {
	IO def;
	Function idxfunc;
	DataBase idxlbl;
	str keysep;
	str labelsep;
};

void ComplexClass::next(str label, str desc)
{
	int n = dim(idxlbl);

	if	(label)
	{
		idxlbl += { label, desc };
		idxfunc.fline("return %d; /* %s */", n, label);
	}
	else	idxfunc.fline("return %d;", n - 1);
}

bool ComplexClass::dummy(str cmd, str lbase, str dbase)
{
	str key, cmd;

	while (getline(def, key, cmd))
	{
		if	(key[0] == '@')
		{
			if	(key == "@end")	break;
			else	dummy(cmd, lbase, dbase);
		}
	}

	return false;
}

bool ComplexClass::switch(str expr, str lbase, str dbase)
{
	str key, name, cmd;
	str last = NULL;
	str label = lbase;
	str desc = dbase;
	bool add_entry = false;
	bool retval = false;

	idxfunc.line();
	idxfunc.line("switch (" + xstrcut(expr, "%s") + ")");
	idxfunc.beg();

	while (getline(def, cmd))
	{
		if	(cmd[0] == '@')
		{
			if	(cmd == "@end")	break;

			if	(cmd == "@label")
			{
				xstrcut(cmd, "%s");
				label = cmd;
			}
			else if	(subfunc(cmd, label, desc))
				add_entry = false;

			continue;
		}

		if	(add_entry)	next(label, desc);

		key = xstrcut(cmd, "%s");

		if	(key == "*")
		{
			key = NULL;
			idxfunc.case(key);
			retval = true;
		}
		else	idxfunc.case(expr ? psub(expr) : key);

		name = xstrcut(cmd, "%s");

		if	(name)
		{
			if	(name == ".")
			{
				label = NULL;
			}
			else	label = paste(keysep, lbase, psub(name));
			
			desc = paste(labelsep, dbase, psub(cmd));
			last = key;
			add_entry = true;
		}
		else	add_entry = false;
	}

	if	(add_entry)	next(label, desc);

	idxfunc.end();
	return retval;
}

bool ComplexClass::subfunc(str cmd, str lbase, str dbase)
{
	str key = xstrcut(cmd, "%s");

	if	(key == "@eval")	eval(cmd);
	else if	(key == "@switch")	return switch(cmd, lbase, dbase);
	else if	(key == "@test")	return dummy(cmd, lbase, dbase);
	else if	(key == "@skip")	return dummy(cmd, lbase, dbase);

	return false;
}

void ComplexClass::main(void)
{
	str cmd;
	bool rval = false;

	while (getline(def, cmd))
	{
		if	(cmd[0] == '@')
		{
			rval = subfunc(cmd, NULL, NULL);
		}
		else	idxfunc.line(cmd);
	}

	if	(!rval)
		idxfunc.fline("return %d;\n", dim(idxlbl));
}

virtual void ClassDef::complex(str name, str desc, IO def)
{
	addentry(name, desc);
	ComplexClass x;
	x.def = def;
	x.idxfunc = func(name);
	x.idxlbl = DataBase(ClassDefLabel);
	x.keysep = ".";
	x.labelsep = ", ";
	x.main();
	x.idxfunc.write(false);
	mklabel(name, x.idxlbl);
}

#endif	/* _MS_CLASSDEF */
