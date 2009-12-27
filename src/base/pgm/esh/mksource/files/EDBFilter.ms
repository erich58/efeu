/*
EDB-Datenbankfilter

$Header	<$1>
$Copyright (C) 2007 Erich Frühstück
A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#ifndef	EDBFilter_ms
#define	EDBFilter_ms	1

#include <macro.ms>
#include <function.ms>

/*
Die Funktion |$1| generiert einen Filter für EDB-Datenbanken.
Als Argument wird die Syntax <syntax> des Filters, ein kurzer
Beschreibungstext <label> und der Funktionsrumpf <read> einer
Lesefunktion benötigt. Der Filtername <name> wird aus der Syntax
abgeleitet.

Die optionalen Argumente haben folgende Bedeutung:
[<type>]
	legt den Datenbanktyp fest, auf den der Filter
	angewendet werden kann. Bei |NULL| kann er auf einen
	beliebigen Datenbanktyp angewendet werden.
[<otype>]
	legt den Datenbanktyp der resultierenden Datenbank fest.
[<par>]
	Falls <par> verschieden von |NULL| ist, wird eine
	Variable vom Typ <par> zur Steuerung des Filters angelegt.
	Bei dem Datentyp <par> muss es sich um einen Referrenztyp
	handeln, ein solcher wird am einfachsten mithilfe der Funktion
	|RefPar| eingerichtet.
[<pfx>]
	ist ein Präfix für den Namen des generirten Filters und der
	Hilfsfunktionen.
[<ext>]
	ist eine boolsche Variable, die bestimmt, ob die
	Filterdefinition statisch oder mit externer Bindung
	eingerichtet wird (Vorgabe: true).

Das letzte Kommentar vor Aufruf der Funktion wird als
Beschreibungstext für den Datenbankfilter verwendet.
Achtung: Kommentare im Funktionsrumpf werden vor dem Aufruf der Funktion
verarbeitet! 

Bei der Aufbereitung der Funktionsrümpfe von <create> und <read>
werden die folgenden Parametersubstitutionen vorgenommen:

[|\$1|]	Der Type der Basisdatenbank;
[|\$2|]	Der Type der resultierenden Datenbank;
[|\$3|]	Der Name der Parameterstruktur.

In der Ledefunktion sind die folgenden Variablen vordefiniert:
|type|
	ist der Datentype des zu lesenden Objektes
|data|
	ist der Pointer auf das zu lesende Objekt
|base|
	ist der Name der Basis-Datenbank und steht nur zur Verfügung,
	wenn kein Parametertyp definiert wurde.
|par|
	ist ein Pointer auf die Parameterstruktur bei Verwendung eines
	Parametertyps.
*/

str EDBFilter (str syntax, str label, str create, str read,
	str type = NULL, str otype = NULL, str par = NULL,
	str pfx = NULL, bool ext = true)
{
	static bool need_hdr = true;

	if	(need_hdr)
	{
		StdInclude(hdr, "EFEU/EDBFilter");
		need_hdr = false;
	}

	syntax = psub(syntax);
	str name = strcut(syntax, "[=", -1);

	name = psub(name);
	type = psub(type);
	otype = psub(otype);
	label = psub(label, name);
	par = psub(par);

	usecomment();

	str vname = paste("_", psub(pfx), name);

	if	(ext)
	{
		fprintf(hdr, "extern EDBFilter %s;\n", vname);
	}
	else	fprintf(src, "static ");

	fprintf(src, "EDBFilter %s = ", vname);
	fprintf(src, "EDB_FILTER(NULL, %#s, %#s,\n\t", name, syntax);
	fprintf(src, "%s_create, NULL,\n\t%#s\n);\n", vname, label);

	Function f = Function("EDB *$1_create", "EDBFilter *def, EDB *base,
	const char *opt, const char *arg", vname);

	f.line("EDB *edb = NULL;");

	if	(par)
		f.line("$1 *par = NULL;", par);

	f.newpar();
	f.body(create, type, otype, par);
	f.newpar();
	f.line("if\t(!edb->desc)");
	f.line("\tedb->desc = mstrcpy(base->desc);");
	f.newpar();
	f.line("edb->read = $1_read;", vname);
	f.line("edb->ipar = $1;", par ? "par" : "base");
	f.line("return edb;");
	f.write(false);

	Function f = Function("int $1_read",
		"EfiType *type, void *$2, void *p_par",
			vname, otype ? "p_data" : "data");

	if	(otype && read == /data/)
		f.line("$1 *data = p_data;", otype);

	if	(par)
	{
		if	(read == /par/)
			f.line("$1 *par = p_par;", par);
	}
	else
	{
		if	(read == /base/)
			f.line("EDB *base = p_par;");
	}

	f.line();
	f.body(read, type, otype, par);
	f.write(false);
	return vname;
}

/*
$Example

Das folgende Beispiel zeigt einen minimalen Filter.
\ms_example dummy
config src

#include <EDBFilter.ms>

/\*
Der Filter \$1 läßt die Daten unverändert.
*\/

EDBFilter("dummy", "Dummy-Filter",
NULL, string !
edb_read(base);
return 1;
!);
\end

$SeeAlso
\mref{mksource(1)}, \mref{vstat(7)}, \mref{TypeDef(7)}.\br
Handbuch <"Die EDB-Datenschnittstelle">.
*/

#endif	/* EDBFilter.ms */
