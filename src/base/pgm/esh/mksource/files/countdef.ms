/*	Zähler generieren
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_MS_COUNTDEF
#define	_MS_COUNTDEF	1

#include <function.ms>
#include <cntobj.ms>

struct CountDefEntry {
	str name;
	str line;
};

struct CountDef {
	str decl;
	DataBase cdef;
};

CountDef CountDef(str decl = NULL)
	return { (decl ? decl : "const void *ptr"), DataBase(CountDefEntry) };

void CountDef::tab(str name)
{
	src << "\n/*\tZählertabelle\n*/\n\n";
	fprintf(src, "\nstatic MdCount_t %s[] = {\n", name);

	for (x in cdef)
		fprintf(src, "\t%s,\n", x.line);

	src << "};\n\n";
}

void CountDef::var(str pfx = NULL)
{
	src << "\n";

	for (x in cdef)
		fprintf(src, "MdCount_t %s%s = %s;\n", pfx, x.name, x.line);
}

void CountDef::setup(str name, bool glob = true)
{
	name = psub(name);
	Function f = Function("void $1", "xtab_t *tab", name);

	if	(dim(cdef) > 0)
	{
		tab(name + "_tab");
		f.line("XAPPEND(tab, $1_tab, XS_ENTER);");
	}
	else	f.line(";");

	f.write(glob);
}


void CountDef::add & (str name, str desc)
{
	cdef += { name, psub("{ $(name:%#s), CT_$1, $(desc:%#s),\n\
\t\t(MdSet_t) set_$1, (MdInit_t) ini_$1, (MdAdd_t) add_$1 }", name) };
}

void CountDef::standard(str name, str desc, Type_t type, IO def)
{
	fprintf(src, "\n/*\t%s\n*/\n\n", desc ? desc : "Zähler " + name);
	CntObj(name, type, def, decl);
	add(name, desc);
}

void CountDef::simple(str name, str desc, Type_t type, str cmd)
	this.standard(name, desc, type, ". . " + cmd);

#endif	/* _MS_COUNTDEF */
