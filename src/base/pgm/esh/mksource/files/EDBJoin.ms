/*
EDB-Datenbankverknüpfung

$Header	<$1>
$Copyright (C) 2007 Erich Frühstück
A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#ifndef	EDBJoin_ms
#define	EDBJoin_ms	1

#include <macro.ms>
#include <function.ms>
#include <RefPar.ms>

/*
Die Funktion |$1| richtet eine Datenstruktur zur Verknüpfung von zwei
Datenbanken ein.

[<t1>]
	legt den Datenbanktyp der ersten Datenbank fest.
[<t2>]
	legt den Datenbanktyp der zweiten Datenbank fest.
[<read>]
	ist der Funktionsrumpf für die Verarbeitungsfunktion.
[<par>]
	Falls <par> verschieden von |NULL| ist, wird eine
	Variable vom Typ <par> zur Steuerung des Filters angelegt.
	Bei dem Datentyp <par> muss es sich um einen Referrenztyp
	handeln, ein solcher wird am einfachsten mithilfe der Funktion
	|RefPar| eingerichtet.
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

[|\$1|]	Der Type der ersten Datenbank;
[|\$2|]	Der Type der zweiten Datenbank;
[|\$3|]	Der Type der resultierenden Datenbank;
[|\$4|]	Der Name der Parameterstruktur.

In der Lesefunktion sind die folgenden Variablen vordefiniert:
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

str EDBJoin (str db1, str db2, str create, str read,
	str type = NULL, str par = NULL, bool ext = true)
{
	static bool need_hdr = true;

	if	(need_hdr)
	{
		StdInclude(hdr, "EFEU/EDBJoin");
		need_hdr = false;
	}

	str desc = usecomment();
	fprintf(ioerr, "desc: %s\n", desc);
	db1 = psub(db1);
	db2 = psub(db2);
	str name = psub("EDBJoin_$1_$2", db1, db2);
	str init;

	if	(ext)
	{
		fprintf(hdr, "extern EDBJoin %s;\n", name);
	}
	else	fprintf(src, "static ");

	fprintf(src, "EDBJoin %s = {\n\t", name);

	for (x in db1, db2, type)
		fprintf(src, "%s, ", x ? "&Type_" + x : "NULL");

	fprintf(src, "%s_create, NULL,\n\t%#s\n};\n", name, desc);

	Function f = Function("EDB *$1_create",
		"EDBJoin *def, EDB *db1, EDB *db2, const char *arg", name);

	f.line("EDB *edb = NULL;");

	if	(par)
	{
		f.line("$1 *par = memalloc(sizeof *par);", par);
	}
	else	f.line("EDBJoinPar *par = EDBJoinPar_create(db1, db2);");

	f.newpar();
	f.body(create, db1, db2, type, par);

	if	(par)
		f.line("rd_init(&$1_reftype, par);", par);

	f.newpar();
	f.line("if\t(!edb)");
	f.beg();
	f.line("rd_deref(db1);");
	f.line("rd_deref(db2);");
	f.line("rd_deref(par);");
	f.line("return NULL;");
	f.end();
	f.newpar();
	f.line("if\t(!edb->desc)");
	f.line("\tedb->desc = mstrpaste(NULL, db1->desc, db2->desc);");
	f.newpar();
	f.line("edb->read = $1_read;", name);
	f.line("edb->ipar = par;");
	f.line("return edb;");
	f.write(false);

	Function f = Function("int $1_read",
		"EfiType *type, void *$2, void *p_par",
			name, type ? "p_data" : "data");

	if	(type && read == /data/)
		f.line("$1 *data = p_data;", type);

	if	(par)
	{
		if	(read == /par/)
			f.line("$1 *par = p_par;", par);
	}
	else
	{
		f.line("EDBJoinPar *par = p_par;");
		f.line("EDB *db1 = par->db1;");
		f.line("EDB *db2 = par->db2;");
	}

	f.line();
	f.body(read, db1, db2, type, par);
	f.write(false);
	return name;
}

/*
$SeeAlso
\mref{mksource(1)}, \mref{vstat(7)}, \mref{TypeDef(7)}.\br
Handbuch <"Die EDB-Datenschnittstelle">.
*/

/**/

#endif	/* EDBJoin.ms */
