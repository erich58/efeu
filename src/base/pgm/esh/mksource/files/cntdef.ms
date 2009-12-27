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

/*
Die Struktur |$1| dient zur Konstruktion von Zählobjekten und
Zählklassifikationen. Sämtliche Hilfsfunktionen und Hilfstabellen
werden statisch eingerichtet. Der Zugriff erfolgt über
Initialisierungsfunktionen, die wahlweise statisch oder global
eingerichtet werden.
*/

struct CntDef {
	bool glob;	/* Flag für globale Definition */
	str pfx;	/* Namensprefix */
	str cntarg;	/* Argumentliste der Zählerfunktionen */
	str ctype;	/* Type der Klassendefinitionen */
	str idxarg;	/* Argumentliste der Klassenfunktionen */
	str idxvar;	/* Variablendeklaration der Klassenfunktionen */
	str ptr;	/* Pointer mit Klassifikationsvariablen */
	str var_type;	/* Variablentype */
	str var_name;	/* Variablenname */
	int var_dim;	/* Dimension des Variablenvektors */
	DataBase ini;	/* Datenbank mit Initialisierungsfunktionen */
	DataBase cnt;	/* Datenbank mit Zählern */
	DataBase class;	/* Datenbank mit Klassifikationen */
};


/*
Die erste Form des Konstruktors richtet
Zähldefinitionen ein, die Parameterwerte ausschließlich über
globale Variablen oder über den mit |md_count| übergebenen Datenpointer
erhalten. Die Variable <type> gibt den Datentype des Datenpointers an.
wird ein Nullstring übergeben, wird <"|const void *|"> verwendet.
*/

virtual inline CntDef CntDef (str type = NULL, bool glob = true,
	str pfx = NULL)
{
	str arg = type ? psub("const $1 *ptr = data;", type) : NULL;
	str decl = type ? psub("const $1 *ptr = data;", type) : NULL;
	return (CntDef) { glob, psub(pfx), arg,
		"MdClass", "MdClass *c, const void *data",
		decl, (type ? "ptr" : "data"), NULL, NULL, 0, DataBase(str),
		DataBase(LabelEntry), DataBase(LabelEntry) };
}

/*
Der zweite Konstruktor verwendet einen Schnittstellenvektor der
Dimension <n> zur Kommunikation mit den Zählfunktionen.
Die Datenstruktur <name> wird mit mithile von |Struct|
(Vergleiche dazu \mref{struct(7)}) aus <def> generiert.
*/

virtual inline CntDef CntDef (str name, int n, str def,
	str vname = NULL, bool glob = true, str pfx = NULL)
{
	name = psub(name);

	if	(def)
	{
		Struct st = Struct(NULL, name)
		st.addentry(psubfilter(def, name));
		IO out = glob ? hdr : src;
		out << "\n/*\tKlassifikationsvariablen\n*/\n";
		st.type(out, name);
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
	str decl = psub("$1 *ptr = ((MdPtrClass *) c)->ptr;", name);

	return (CntDef) { glob, psub(pfx), NULL,
		"MdPtrClass", "MdClass *c, const void *data",
		decl, "ptr", name, vname, n, DataBase(str),
		DataBase(LabelEntry), DataBase(LabelEntry) };
}


/*
Mithilfe von |CntDef::init| kann eine
Initialisierungsfunktion für die Zählvariablen eingerichtet werden.
Die Initialisierungsfunktion hat als erstes Argument den Index auf
den Schnittstellenvektor und liefert den entsprechenden Pointer.
*/

void CntDef::init(str name, str args = NULL, str body = NULL)
{
	List_t parg = {var_type, var_name, var_dim ? var_dim : 1};
	Function f = Function("$1 *" + name, paste(", ", "int n", args), parg);
	f.line("static $1 buf, *ptr;", parg);
	f.newpar();
	f.line("ptr = n < $3 ? $2 + n : &buf;", parg);
	f.line("memset(ptr, 0, sizeof($1));", parg);

	if	(body)
	{
		f.line();
		f.beg();
		f.body(body, parg);
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

	if	(pext)	pext = ", " + psub(pext, name);

	fprintf(src, "static %s %s[] = {\n", ctype, name);

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

/*
Die Funktion |CntDef::setup| richtet statische Vektoren mit den
Zählobjekten und den Klassifikationen ein. Weiters konstruiert
sie eine Funktion zum Eintragen der entsprechenden Einträge
in die Zähldatenbanken.
*/

void CntDef::setup(str name, str lblpfx = NULL, ...)
{
	name = psub(name, va_list);
	Function f_ext = var_type && (glob || var_dim) ? \
		Function("void $1_extend",
		"MdClassTab *class, const char *ext, $2 *ptr",
		name, var_type) : NULL;
	Function f_add = Function("void $1_addclass",
		"MdClassTab *class, int n", name);
	Function f = Function("void $1_setup",
		"MdCntObjTab *count, MdClassTab *class, int n", name);

	for (x in ini)
		f.line(x);

	ini.delete(0, dim(ini));

	if	(dim(cnt))
	{
		src << "\n/*\tZählertabelle\n*/\n\n";
		fprintf(src, "static MdCntObj %s_cnt[] = {\n", name);

		for (x in cnt)
		{
			fprintf(src, "\t{ %#s, ", x.name); 
			fprintf(src, "CT_%s, ", x.name); 
			fprintf(src, "%#s,\n\t\t", x.desc); 
			fprintf(src, "set_%s, ", x.name);
			fprintf(src, "ini_%s, ", x.name);
			fprintf(src, "add_%s },\n", x.name);
		}

		src << "};\n\n";
		f.line("MdCntObj_add(count, $1_cnt, tabsize($1_cnt));", name);
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

		f_add.switch("n");
		f_add.case("0", "$1_add(class, $2_class0, $3); break;",
			ctype, name, n);

		f.line("$1_addclass(class, 0);", name);
		ctab(cname + "0", lbl, var_name, xdesc);

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

			if	(!ext || ext == ".")
				ext = sprintf("%d", i + 1); 

			str var = sprintf("%s + %d", var_name, i);
			f_add.case(i, "$1_extend(class, $2); break;",
				name, sprintf("%#s, %s", ext, var));
		}

		f_add.case(NULL, "break;");
		f_add.end();

		f_ext.line("$1_xadd(class, $2_class0, $3, ext, ptr);",
			ctype, name, n);

		class.delete(0, dim(class));
	}
	else
	{
		f_ext.line(";");
		f_add.line(";");
	}

	f_ext.write(glob);
	f_add.write(glob);
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

/*
\shead	Zählobjekt

Ein Zählobjekt wird mit der Funktion |CntDef::cntobj| eingerichtet.
Dies erfolgt mit der Funktion |CntObj| (Vergleiche dazu
\mref{cntobj(7)}).
*/

void CntDef::cntobj(str name, str desc, Type_t type, IO def)
{
	add_cnt(name, desc);
	str ptr = var_dim ? var_name : "data";
	CntObj(name, type, def, cntarg);
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
	stdfunc(name, "return (" + expr + ") ? 0 : 1;");
	Label lbl = Label(name);
	lbl.add((fmt ? fmt : name), desc);
	lbl.tab(false);
}

/*
[|CntDef::switch|]
	konstruiert eine Klassifikation mithilfe eines
	|switch|-Blocks.
*/

virtual void CntDef::switch(str name, str desc, str expr, str fmt, IO def)
{
	add_class(name, desc);
	IndexFunc f = idxfunc(name);

	if	(!f.switch(expr, fmt, def))
		f.func.fline("return %d;", dim(f.lbl));
		
	f.write(false);
}

virtual void CntDef::switch(str name, str desc, str expr, IO def)
	this.switch(name, desc, expr, "$(key)", def);

/*
[|CntDef::test|]
	konstruiert eine Klassifikation mithilfe von
	|if|-Anweisungen.
*/

virtual void CntDef::test(str name, str desc, str init, str fmt, IO def)
{
	add_class(name, desc);
	IndexFunc f = idxfunc(name);
	f.func.body(init);

	if	(!f.test(fmt, def))
		f.func.fline("return %d;", dim(f.lbl));
		
	f.write(false);
}

virtual void CntDef::test(str name, str desc, str init, IO def)
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
.	val	$(ptr)->satz
return 	.	$(ptr)->satz ? 1 : 0
!);

cdef.test("std", "Standardklassen",
	"int x = $(ptr)->satz;", "x <= $(key)", string !
0
@test	$(ptr)->btag == $(key)
0	B0	Keine Beitragstage
*	E0	Kein Einkommen
@end
10000	E$(key)	Einkommen $((int) last + 1) bis zu $(key)
20000	E$(key)	Einkommen $((int) last + 1) bis zu $(key)
50000	E$(key)	Einkommen $((int) last + 1) bis zu $(key)
*	EXXX	Einkommen über $(last)
!);

cdef.setup("EK", "ek");
\end
*/

#endif	/* cntdef.ms */
