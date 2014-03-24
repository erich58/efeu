/*
Zählerdefintion generieren

$Header	<$1>
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
#include <cntvar.ms>

/*
Die Struktur |$1| dient zur Konstruktion von Zählobjekten und
Zählklassifikationen. Sämtliche Hilfsfunktionen und Hilfstabellen
werden statisch eingerichtet. Der Zugriff erfolgt über
Initialisierungsfunktionen, die wahlweise statisch oder global
eingerichtet werden.
*/

struct CntDef {
	bool glob;	/* Flag für globale Definition */
	str tsetup;	/* Datentypinitialisierung */
	str ftab;	/* Funktionstabelle */
	str pfx;	/* Namensprefix */
	str ctype;	/* Type des Zählobjektes */
	str cntarg;	/* Argumentliste der Zählerfunktionen */
	str idxtype;	/* Type der Klassendefinitionen */
	str idxarg;	/* Argumentliste der Klassenfunktionen */
	str idxvar;	/* Variablendeklaration der Klassenfunktionen */
	str ptr;	/* Pointer mit Klassifikationsvariablen */
	str var_type;	/* Variablentype */
	str var_name;	/* Variablenname */
	int var_dim;	/* Dimension des Variablenvektors */
	str[] add;	/* Vektor für Ergänzungen */
	str[] ini;	/* Vektor für Initialisierungen */
	DataBase cnt;	/* Datenbank mit Zählern */
	DataBase class;	/* Datenbank mit Klassifikationen */
	bool compat;	/* Kompatiblitätsflag (kein Prefix für Zählobjekte) */
};


/*
Die erste Form des Konstruktors richtet
Zähldefinitionen ein, die Parameterwerte ausschließlich über
globale Variablen oder über den mit |md_count| übergebenen Datenpointer
erhalten. Die Variable <type> gibt den Datentype des Datenpointers an.
wird ein Nullstring übergeben, wird <"|const void *|"> verwendet.
*/

virtual promotion inline CntDef CntDef (str type = NULL, bool cglob = true,
	str cpfx = NULL)
{
	glob = cglob;
	pfx = psub(cpfx);
	ctype = "MdCount";
	cntarg = type ? psub("const $1 *ptr = data;", type) : NULL;
	idxtype = "MdClass";
	idxarg = "MdClass *c, const void *data";
	idxvar = type ? psub("const $1 *ptr = data;", type) : NULL;
	ptr = type ? "ptr" : "data";
	cnt = DataBase(LabelEntry);
	class = DataBase(LabelEntry);
}

/*
Der zweite Konstruktor verwendet einen Schnittstellenvektor der
Dimension <n> zur Kommunikation mit den Zählfunktionen.
Die Datenstruktur <name> wird mit mithilfe von |Struct|
(Vergleiche dazu \mref{struct(7)}) aus <def> generiert.
*/

virtual promotion inline CntDef CntDef (str name, int n, str def,
	str vname = NULL, bool cglob = true, str cpfx = NULL)
{
	name = psub(name);
	glob = cglob;
	pfx = psub(cpfx);

	if	(def == /[[:space:]]/)
	{
		Struct st = Struct(name);
		st.addentry(psubfilter(def, name));
		IO out = glob ? hdr : top;
		out << "\n/*\tKlassifikationsvariablen\n*/\n";
		st.type(out, name);
		tsetup = psub("$1_mktype", name);
		st.setup(tsetup, glob);
	}
	else if	(def)
	{
		fpsub(glob ? hdr : top, "typedef $1 $2;\n", def, name);
	}

	if	(!vname)
	{
		src << "static ";
		vname = name + "_var";
	}
	else if	(!glob)
	{
		src << "static ";
	}
	else	fpsub(hdr, "extern $1 $2[$3];\n", name, vname, n);

	fpsub(src, "$1 $2[$3];\n", name, vname, n);
	cntarg = psub("$1 *ptr = ((MdPtrCount *) cnt)->ptr;", name);
	idxvar = psub("$1 *ptr = ((MdPtrClass *) c)->ptr;", name);

	if	(tsetup)
	{
		Function f = Function("void f_cntvar_$1",
			"EfiFunc *func, void *rval, void **arg",
			name);
		f.line("if\t(Val_int(arg[0]) < $1)", n ? n : 1);
		f.line("\tCopyData(&Type_$1, $2 + Val_int(arg[0]), arg[1]);",
			name, vname);
		f.write(false);
		ftab += "\t{ FUNC_VIRTUAL, &Type_void, ";
		ftab += psub("\"cntvar (int, $1)\", f_cntvar_$1 },\n", name);
	}

	ctype = "MdPtrCount";
	idxtype = "MdPtrClass";
	idxarg = "MdClass *c, const void *data";
	ptr = "ptr";
	var_type = name;
	var_name = vname;
	var_dim = n;
	cnt = DataBase(LabelEntry);
	class = DataBase(LabelEntry);
}


/*
Mithilfe von |CntDef::init| kann eine
Initialisierungsfunktion für die Zählvariablen eingerichtet werden.
Die Initialisierungsfunktion hat als erstes Argument den Index auf
den Schnittstellenvektor und liefert den entsprechenden Pointer.
*/

void CntDef::init(str name, str args = NULL, str body = NULL)
{
	List_t parg = {var_type ? var_type : "void",
		var_name, var_dim ? var_dim : 1};
	name = psub(name, parg);
	Function f = Function("$1 *" + name, paste(", ", "int n", args), parg);

	if	(var_type)
	{
		f.line("static $1 buf, *ptr;", parg);
		f.newpar();
		f.line("ptr = n < $3 ? $2 + n : &buf;", parg);
	}

	if	(body)
	{
		f.line();
		f.beg();
		f.body(body, parg);
		f.end();
		f.line();
	}
	else	f.line("memset(ptr, 0, sizeof($1));", parg);

	if	(var_type)
		f.line("return ptr;");
	else	f.line("return NULL;");

	f.write(glob);
}

void CntDef::ctab(str name, str pfx, str ext = NULL,
	str pext = NULL, str xdesc = NULL)
{
	src << "\n/*\tZählertabelle\n*/\n\n";
	name = psub(name);

	if	(pext)	pext = ", " + psub(pext, name);
	if	(ext)	ext = "." + ext;

	fprintf(src, "static %s %s[] = {\n", ctype, name);

	for (x in cnt)
	{
		str cname = paste(".", pfx, x.name) + ext;
		str fname = x.name;
		fprintf(src, "\t{ %#s, CT_%s, %#s,\n\t", cname, fname,
			paste(", ", x.desc, xdesc)); 
		fpsub(src, "\tset_$1, ini_$1, add_$1", fname);
		src << pext << " },\n";
	}

	src << "};\n\n";
}

void CntDef::idxtab(str name, str fmt = NULL, str pext = NULL, str xdesc = NULL)
{
	src << "\n/*\tKlassifikationstabelle\n*/\n\n";
	name = psub(name);

	if	(pext)	pext = ", " + psub(pext, name);

	fprintf(src, "static %s %s[] = {\n", idxtype, name);

	for (x in class)
	{
		str cname = fmt ? psub(fmt, x.name) : x.name;
		str fname = paste("_", pfx, x.name);
		fprintf(src, "\t{ %#s, %#s,\n\t", cname,
			paste(", ", x.desc, xdesc)); 
		fpsub(src, "\t$1_dim, $1_tab, $1_idx", fname);
		src << pext << " },\n";
	}

	src << "};\n\n";
}

void CntDef::addfunc(str name, str args, str body, ...)
{
	name = psub(name, va_list);
	Function f = Function("void f_$1",
		"EfiFunc *func, void *rval, void **arg", name);
	f.body(body, va_list);
	f.write(false);
	ftab += psub("\t{ 0, &Type_void, \"$1 ($2)\", f_$1 },\n", name, args);
}

void CntDef::xinit(str name, str args, str body, ...)
{
	name = psub(name, va_list);
	Function f = Function("void f_$1_$2",
		"EfiFunc *func, void *rval, void **arg", var_type, name);
	f.line("$1 *ptr = arg[0];", var_type);
	f.body(body, va_list);
	f.write(false);
	ftab += psub("\t{ 0, &Type_void, \"$1::$2 & ($3)\", f_$1_$2 },\n",
		var_type, name, args);
}

void CntDef::create(str name, str args, str body, ...)
{
	name = psub(name, va_list);
	Function f = Function("void f_$1_$2",
		"EfiFunc *func, void *rval, void **arg", var_type, name);
	f.line("$1 *ptr = rval;", var_type);
	f.body(body, va_list);
	f.write(false);
	ftab += psub("\t{ 0, &Type_$1, \"$1 ($3)\", f_$1_$2 },\n",
		var_type, name, args);
}

/*
Die Funktion |CntDef::setup| richtet statische Vektoren mit den
Zählobjekten und den Klassifikationen ein. Weiters konstruiert
sie eine Funktion zum Eintragen der entsprechenden Einträge
in die Zähldatenbanken.
*/

void CntDef::setup(str name, str lblpfx = NULL, ...)
{
	int i, n;

	name = psub(name, va_list);
	Function f_ext = var_type && (glob || var_dim) ? \
		Function("void $1_extend",
			"MdCountPar *count, const char *ext, $2 *ptr",
			name, var_type) : NULL;
	Function f_add = Function("void $1_add",
		"MdCountPar *count, int n", name);
	Function f = Function("void $1_setup",
		"MdCountPar *count, int n", name);

	for (x in add)
		f_add.body(x);

	for (x in ini)
		f.body(x);

	ini.delete(0, dim(ini));

	str xdesc = va_list.pop();
	str ext = strcut(xdesc, "%s");
	str fmt = paste(".", lblpfx, "$1");
	str lbl = paste(".", fmt, ext == "." ? NULL : ext);

	f_add.switch("n");
	f_add.case("0");

	if	((n = dim(cnt)))
	{
		ctab(name + "_count0", compat ? NULL : lblpfx,
			ext == "." ? NULL : ext, var_name, xdesc);
		f_add.line("$1_add(count, $2_count0, $3);",
			ctype, name, n);
		f_ext.line("$1_xadd(count, $2_count0, $3, ext, ptr);",
			ctype, name, n);
		cnt.delete(0, dim(cnt));
	}

	if	((n = dim(class)))
	{
		idxtab(name + "_class0", lbl, var_name, xdesc);
		f_add.line("$1_add(count, $2_class0, $3);",
			idxtype, name, n);
		f_ext.line("$1_xadd(count, $2_class0, $3, ext, ptr);",
			idxtype, name, n);
		class.delete(0, dim(class));
	}

	f_add.line("break;");

	for (i = 1; i < var_dim; i++)
	{
		xdesc = va_list.pop();
		ext = strcut(xdesc, "%s");

		if	(!ext || ext == ".")
			ext = sprintf("%d", i + 1); 

		str var = sprintf("%s + %d", var_name, i);
		f_add.cblock(i, "$1_extend(count, $2);\nbreak;",
			name, sprintf("%#s, %s", ext, var));
	}

	f_ext.write(glob);

	f_add.cblock(NULL, "break;");
	f_add.end();
	f_add.write(glob);
	f.line("$1_add(count, 0);", name);

	if	(var_dim > 1)
	{
		f.line();
		f.line("while\t(n-- > 1)");
		f.line("\t$1_add(count, n);", name);

	}

	f.write(glob);

	if	(!tsetup)	return;

	addfunc("$1_setup", "CountTab, int",
		"$1_setup(Val_ptr(arg[0]), Val_int(arg[1]));",
		name);

	fpsub(src, "static EfiFuncDef $1_fdef[] = {\n", name);
	src << ftab;
	src << "};\n";

	f = Function("void Setup_$1", "void", name);
	f.line("$1();", tsetup);
	f.line("AddFuncDef($1_fdef, tabsize($1_fdef));", name);
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

void CntDef::extend(str type, str ext, str var)
{
	type = psub(type);
	ext = psub(ext);
	var = psub(var);

	if	(var[0] != '&')
	{
		fprintf(src, "static %s %s;\n", type, var);
		var = "&" + var;
	}

	ini.append(sprintf("%s_extend(count, %#s, %s);",
		type, ext, var));
}

void CntDef::use(...)
{
	for (x in va_list)
	{
		str n = x;
		str name = strcut(n, "[");

		if	(n)
		{
			ini.append(psub("$1_add(count, $2);",
				name, (int) n));
		}
		else
		{
			add.append(psub("$1_add(count, n);", name));
			ini.append(psub("$1_setup(count, n);", name));
		}
	}
}

/*
\shead	Zählobjekt

Ein Zählobjekt wird mit der Funktion |CntDef::cntobj| eingerichtet.
Dies erfolgt mit der Funktion |CntObj| (Vergleiche dazu
\mref{cntobj(7)}).
*/

void CntDef::cntobj(str name, str desc, Type_t type, IO def)
{
	add_cnt(name, desc);
	CntObj(name, type, def, cntarg);
}

/*
Zählobjekte zur Ausgabe von Variablen können auch mit der Funktion
|CntDef::cntvar| eingerichtet werden. Die Verwendung ist äquivalent
zu einem Aufruf von |CntObj| mit der Definition
	|set . $2 =| <expr>\br
	|add . \$1 = $2|
*/

virtual void CntDef::cntvar(str name, str desc, Type_t type, str expr)
{
	add_cnt(name, desc);
	CntObj(name, type, "set . $2 = " + expr + "\nadd . $1 = $2", cntarg);
}

virtual void CntDef::cntvar(str name, str desc, str type, str expr)
{
	add_cnt(name, desc);
	CntVar(name, type, expr, cntarg);
}

/*
\shead	Klassifikationsfunktionen
*/

inline Function CntDef::func(str name, ...)
{
	Function f = Function("int " + name + "_idx", idxarg, va_list)
	f.line(idxvar);
	return f;
}

inline IndexFunc CntDef::idxfunc(str name)
{
	IndexFunc f = IndexFunc(paste("_", pfx, name), idxarg);
	f.func.line(idxvar);
	return f;
}

void CntDef::stdfunc(str name, str fdef, ...)
{
	Function f = func(name, va_list);
	f.body(fdef, va_list);
	f.write(false);
}

/*
[|CntDef::classfunc|]
	definiert eine frei gestaltete Klassifikationsfunktion.
	Die Variable <fdef> definiert den Funktionsrumpf und
	die Eingabestruktur <def> definiert die Klassenkennungen.
	Jede Zeile von <def> enthält eine Kennung und einen
	Beschreibungstext.

*/

void CntDef::classfunc(str name, str desc, str fdef, IO def)
{
	add_class(name, desc);
	name = paste("_", pfx, name);
	stdfunc(name, fdef);
	Label lbl = Label(name);
	lbl.load(def);
	lbl.tab(false);
}

/*
[|CntDef::simple|]
	generiert eine einfache Klassifikationsfunktion mit <n>
	Klassen. Der Rückgabewert der Funktion wird vom Ausdruck
	<expr> bestimmt. Die Variablen <fmt> und <lbl> dienen zur
	Konstruktion der Kennungen und Beschreibungen.
*/

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

/*
[|CntDef::limit|]
	generiert eine lineare Klassifikation mit einer offenen Klasse
	am Ende. Der Ausdruck <expr> liefert den Klassenindex.
*/

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

/*
[|CntDef::range|]
	generiert eine lineare Klassifikation mit einer offenen Klasse
	am Anfang und am Ende. Der Ausdruck <expr> liefert den Klassenindex.
	Die Variablen <first> und <last> bestimmen die untere, bzw.
	obere Grenze für die zwei offenen Klassen.
*/

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

/*
[|CntDef::flag|]
	liefert eine Klassifikation mit einer Ausprägung,
	die nur Daten zählt, für die der Ausdruck <expr> wahr ist.
*/

void CntDef::flag(str name, str desc, str expr, str fmt = NULL)
{
	add_class(name, desc);
	name = paste("_", pfx, name);
	stdfunc(name, expr ? "return (" + expr + ") ? 0 : 1;" : "return 0;");
	Label lbl = Label(name);
	lbl.add((fmt ? fmt : name), desc);
	lbl.tab(false);
}

/*
[|CntDef::switch|]
	konstruiert eine Klassifikation mithilfe eines
	|switch|-Blocks.
*/

virtual promotion void CntDef::switch(str name, str desc, str expr, str fmt, IO def)
{
	add_class(name, desc);
	IndexFunc f = idxfunc(name);

	if	(!f.switch(expr, fmt, def))
		f.func.fline("return %d;", f.lbl.entry.dim);
		
	f.write(false);
}

virtual promotion void CntDef::switch(str name, str desc, str expr, IO def)
	this.switch(name, desc, expr, "$(key)", def);

/*
[|CntDef::test|]
	konstruiert eine Klassifikation mithilfe von
	|if|-Anweisungen.
*/

virtual promotion void CntDef::test(str name, str desc, str init, str fmt, IO def)
{
	add_class(name, desc);
	IndexFunc f = idxfunc(name);
	f.func.body(init);

	if	(!f.test(fmt, def))
		f.func.fline("return %d;", f.lbl.entry.dim);
		
	f.write(false);
}

virtual promotion void CntDef::test(str name, str desc, str init, IO def)
	test(name, desc, init, "$(key)", def);

/*
[|CntDef::complex|]
	erlaubt die Konstruktion einer Klassifikation, bei der
	|switch|-Blöcke und |if|-Anweisungen beliebig kombiniert
	und verschachtelt werden können.
*/

void CntDef::complex(str name, str desc, IO def)
{
	add_class(name, desc);
	IndexFunc f = idxfunc(name);
	f.body(def);
	f.write(false);
}

/*
$Examples
Das erste Beispiel zeigt die Konstruktion eines Zählobjektes für
Fälle und eine Klasse für die Gesamtheit.

\ms_example
config hdr src

#include <macro.ms>
#include <cntdef.ms>

StdInclude(hdr, "EFEU/mdcount")
Include(src, HeaderName)

CntDef cdef = CntDef()
cdef.cntobj("ges", "Fallzähler", int, ". . 1");
cdef.classfunc("ges", "Gesamtheit", "return 0;", "GES Gesamtheit");
cdef.setup("Simple");
\end

Das zweite Beispiel zeigt eine Zähldefinition mit einer
Zählvariablenstruktur.

\ms_example	eink
config hdr src

#include <macro.ms>
#include <cntdef.ms>

StdInclude(hdr, "EFEU/mdcount")
Include(src, HeaderName)

CntDef cdef = CntDef("EKData", 1, string !
int	btag	Beitragstage
int	satz	Einkommenssatz
!);

cdef.init("SetEK", "unsigned btag, unsigned satz", string !
ptr->btag = btag;
ptr->satz = satz;
!);

struct DezilBasis {
	double anz;
	double val;
};

cdef.cntobj("eink", "Einkommenszähler", DezilBasis, string !
.	anz	1.
.	val	$$(ptr)->satz
return 	.	$$(ptr)->satz ? 1 : 0
!);

cdef.test("std", "Standardklassen",
	"int x = $$(ptr)->satz;", "x <= $$(key)", string !
0
@test	$$(ptr)->btag == $$(key)
0	B0	Keine Beitragstage
*	E0	Kein Einkommen
@end
10000	E$$(key)	Einkommen $$((int) last + 1) bis zu $$(key)
20000	E$$(key)	Einkommen $$((int) last + 1) bis zu $$(key)
50000	E$$(key)	Einkommen $$((int) last + 1) bis zu $$(key)
*	EXXX	Einkommen über $$(last)
!);

cdef.setup("EK", "ek");
\end

$SeeAlso
\mref{mksource(1)}, \mref{struct(7)}, \mref{index(7)}, \mref{label(7)}.
*/

#endif	/* cntdef.ms */
