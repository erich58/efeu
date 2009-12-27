/*
Zählobjekt generieren

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

#ifndef	cntobj_ms
#define	cntobj_ms	1

#include <function.ms>

/*
Die Funktion |$1| dient zur Konstruktion der Komponenten
eines Zählobjektes. Sie benötigt als Argumente einen Namen <name>
einen esh-Datentype <type> und eine Eingabestruktur <def> mit
Definitionszeilen. Mit <arg> kann optional der Zähldatentype (Argument von
|md_count|) angegeben werden. Standardmäßig wird |const void *data|
verwendet.

Es werden folgende Funktionen und Makros
definiert:

\ttitem	CT_<name>
	ist ein Makro mit einem String, der den Datentype <type>
	repräsentiert.
\ttitem	ini_<name>
	ist eine Funktion zur Initialisierung eines Elements der
	Datenmatrix. Wird keine besondere Initialisierungsfunktion
	benötigt, wird stellvertretend ein gleichnamiger Makro
	mit dem Wert |NULL| eingerichtet.
\ttitem	set_<name>
	ist eine Funktion zum Setzen der Werte in einem Buffer.
\ttitem	add_<name>
	ist eine Funktion, die den Wert des Buffers
	in eine Zelle der Datenmatrix addiert.

Alle Makros und Funktionen werden statisch generiert, da sie
nie direkt verwendet werden sondern nur zur Initialisierung
einer Variablen vom Type |MdCount| dienen.

Die einzelnen Zeilen der Eingabestruktur <def> bestehen aus
einem Schlüsselwort <key>, einer Komponentenkennung <var>
und einen Ausdruck <cmd>.

Bei der Komponentenkennung handelt es sich entweder um den Namen
eines Strukturelementes oder einem Punkt für den gesamten Datentype.
Die Stringregister 1 und 2 werden mit Ausdrücken initialisiert,
die auf das gewünschte Element der Datenmatrix, bzw. dem statischen
Buffer verweisen. Auf den Ausdruck wird Parametersubstitution
angewendet.

Das Schlüsselwort bestimmt, für welche Komponentenfunktion
der Ausdruck bestimmt ist. Folgende Kennungen sind möglich:

[|return|] bestimmt den Rückgabewert der Funktion |set_|<name>.
[|set|]	generiert eine Befehlszeile der Funktion |set_|<name>.
[|ini|]	generiert eine Befehlszeile der Funktion |ini_|<name>.
[|add|]	generiert eine Befehlszeile der Funktion |add_|<name>.

Ansonsten definiert <cmd> den Zuweisungswert der Komponente <var>
in der Funktion |set_|<name>. Zusätzlich wird auch eine Befehlszeile
für |ini_|<name> (Initialisierung mit 0) und |add_|<name>
(|+=|-Operator) generiert. Dies ist gleichbedeutend mit:
	\code
	ini	<var>	$1 = 0\br
	set	<var>	$2 = <cmd>\br
	add	<var>	$1 += $2\br

Nur wenn die Kennung |ini| verwendet wurde, wird auch eine Funktion
|ini_|<name> generiert, da ansonsten die Datenmatrix
wesentlich effizienter mit |memset| initialisiert werden kann.

Die Funktion |$1| wurde |inline| definiert, damit die
lokalen Variablen der rufenden Funktion bei der Parametersubstitution
sichtbar bleiben.
*/

inline void CntObj(str name, Type_t type, IO def, str arg = NULL)
{
	fprintf(src, "\n#define\tCT_%s", name);

	str head = typehead(type);

	if	(head)
		fprintf(src, "\t%#s\\\n", "[" + head + "]");

	fprintf(src, "\t%#s\n", declare(type, true));
	
	Function f_set = Function("int set_" + name,
		"MdCount *cnt, void *opaque_buf, const void *data");
	f_set.line("char *buf = opaque_buf;");

	if	(arg)	f_set.line(arg);

	Function f_ini = Function("void ini_" + name,
		"MdCount *cnt, void *opaque_ptr, size_t *idx, size_t dim");
	f_ini.line("char *ptr = opaque_ptr;");
	Function f_add = Function("void add_" + name,
		"MdCount *cnt, void *opaque_ptr, void *opaque_buf");
	f_add.line("char *ptr = opaque_ptr;");
	f_add.line("char *buf = opaque_buf;");
	bool need_ini = false;
	bool need_ret = true;

	str fmt, key, var, cmd;

	while (getline(def, key, var, cmd))
	{
		if	(var[0] == '*')
		{
			expr(paste(" ", type, "x"));
			var = sprintf("x.%s[0]", psub(substr(var, 1)));
			fmt = sprintf("((%s *) (%%s + %d))",
				expr("typeof(" + var + ").cname"),
				offset(x, expr(var)));
		}
		else if	(var[0] != ".")
		{
			expr(paste(" ", type, "x"));
			var = paste(".", "x", psub(var));
			fmt = sprintf("((%s *) (%%s + %d))[0]",
				expr("typeof(" + var + ").cname"),
				offset(x, expr(var)));
		}
		else if	(strlen(var) > 1)
		{
			fmt = sprintf("((%s *) (%%s))->%s", type.cname,
				substr(var, 1));
		}
		else	fmt = sprintf("((%s *) (%%s))[0]", type.cname);

		List_t args = { sprintf(fmt, "ptr"), sprintf(fmt, "buf") };

		switch (key)
		{
		case "return":
			need_ret = false;
			f_set.line("return " + cmd + ";", args);
			break;
		case "set":
			f_set.line(cmd + ";", args);
			break;
		case "ini":
			need_ini = true;
			f_ini.line(cmd + ";", args);
			break;
		case "add":
			f_add.line(cmd + ";", args);
			break;
		case "assign":
			f_set.line("CleanData(&Type_$1, buf);", type);
			f_set.line("Val_$1(buf) = " + cmd + ";", type);
			f_add.line("AssignData(&Type_$1, ptr, buf);", type);
			break;
		default:
			f_ini.line("$1 = 0;", args);
			f_set.line("$2 = " + cmd + ";", args);
			f_add.line("$1 += $2;", args);
			break;
		}
	}

	f_set.line("buf = NULL;");

	if	(need_ret)
		f_set.line("return 1;");

	if	(need_ini)
		f_ini.write(false);
	else	fprintf(src, "#define\tini_%s\tNULL\n", name);

	f_add.line("ptr = NULL;");
	f_add.line("buf = NULL;");
	f_set.write(false);
	f_add.write(false);
}

/*
$Example

Das folgende Beispiel zeigt die Konstruktion eines Zählobjektes
für Dauerverteilungen. Die Dauer einer Episode wird über eine
globale Variable übergeben.

\ms_example
config src

#include <cntobj.ms>

src << "extern unsigned dauer;\n";

struct Dauer {
	int anz;
	int val;
};

CntObj("dauer", Dauer, string !
.	anz	dauer ? 1 : 0
.	val	dauer
return	.	dauer ? 1 : 0
!);
\end
*/

#endif	/* cntobj.ms */
