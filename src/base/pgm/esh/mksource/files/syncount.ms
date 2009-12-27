/*	Synthesis-Zähldefinitionen
	(c) 1998 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#ifndef	_MS_SYNCOUNT
#define	_MS_SYNCOUNT	1

#include <syntools.ms>
#include <countdef.ms>
#include <classdef.ms>
#include <cntdef.ms>

struct SynCount {
	DataBase edef;
};

SynCount SynCount(void)
	return { DataBase(str) };

void SynCount::tab(str name)
{
	src << "\n/*\tAuswertungsfunktionen\n*/\n\n";
	fprintf(src, "\nstatic SynCountDef_t %s[] = {\n", name);

	for (x in edef)
		fprintf(src, "\t%s,\n", x);

	src << "};\n\n";
}

void SynCount::setup(str name, bool glob = true)
{
	Function f = Function("void $1", "xtab_t *tab", name);

	if	(dim(cdef) > 0)
	{
		tab(name + "_tab");
		f.line("XAPPEND(tab, $1_tab, XS_ENTER);");
	}
	else	f.line(";");

	f.write(glob);
}

str SynCount::cntinit (str name, List_t list)
{
	if	(!dim(list))	return "NULL";

	name = "init_" + name;
	Function f = Function("void $1", "xtab_t *count, xtab_t *class", name);
	
	for (z in list)
	{
		str n = z;
		str x = strcut(n, ":");
		f.line("$1_setup(count, class, $2);", x, (int) n);
	}
	
	f.write(false);
	return name;
}

void SynCount::data(str name, str desc, str func, str axis = NULL, ...)
{
	str info = lastcomment(cin);
	LocalVar("DataCount_t cdef_$1", "{ $2, $3, NULL, $4 }", NULL,
		name, func, (axis ? axis : "NULL"), sprintf("%#s", info));
	edef += sprintf("{ %#s, %#s, %s, DataCount_func, %s,
		DataCount_init, DataCount_axis, DataCount_exit, &cdef_%s }",
		name, desc, (info ? "DataCount_info" : "NULL"),
		cntinit(name, va_list), name);
}

void SynCount::range(str name, str desc, str func, str axis = NULL, ...)
{
	name = psub(name);
	desc = psub(desc);
	func = psub(func);
	axis = psub(axis);
	str info = lastcomment(cin);
	LocalVar("RangeCount_t cdef_$1", "{ $2, $3, NULL, $4 }", NULL,
		name, func, (axis ? axis : "NULL"), sprintf("%#s", info));
	edef += sprintf("{ %#s, %#s, %s, RangeCount_func, %s,
		RangeCount_init, RangeCount_axis, RangeCount_exit, &cdef_%s }",
		name, desc, (info ? "DataCount_info" : "NULL"),
		cntinit(name, va_list), name);
}

ClassDef SynCountClass(str name, bool glob = true)
{
	setreg(1, name);
	ClassType(glob ? hdr : src, "$1_class_t", "$1_t *ptr");
	return ClassDef(NULL, "$1_class_t *c", "c->ptr");
}

void SynCountInit(str name, str cdef, str count, ClassDef class,
	bool glob = true, str pfx = NULL)
{
	Function f;

	pfx = psub(pfx);

	f = Function("void $1_init",
		"xtab_t *cdef, xtab_t *count, xtab_t *class", name);

	if	(cdef)	f.line(cdef + "(cdef);");
	if	(count)	f.line(count + "(count);");

	if	(class.type && dim(class.db))
	{
		str fmt = paste(".", pfx, "$1");
		f.line("XAPPEND(class, $1_class_tab, XS_ENTER);", name);
		class.tab("$1_class", glob, fmt, NULL);
	}

	f.write(glob);

/*	TODO	Kompatiblitätsfunktion
*/
	f = Function("void $1_setup",
		"xtab_t *count, xtab_t *class, int n", name);
	f.line("$1_init(NULL, count, class);");
	f.write(glob);

}

#endif	/* _MS_SYNCOUNT */
