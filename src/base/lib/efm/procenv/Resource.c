/*
Kommandoresourcen

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/Resource.h>
#include <EFEU/CmdPar.h>
#include <EFEU/mstring.h>
#include <EFEU/procenv.h>
#include <EFEU/Info.h>
#include <EFEU/Debug.h>
#include <ctype.h>

/*
Die Funktion |$1| analysiert die Kommandozeilenargumente <argv>
und setzt die entsprechenden Resourcen.
Die abgefragten Optionen und Argumente werden aus dem Vektor <argv>
gelöscht und <argc> wird entsprechend justiert.
Der Aufrufname des Kommandos in <argv>|[0]| bleibt erhalten.
*/

void ParseCommand (int *argc, char **argv)
{
	CmdPar *par;

	SetProgName(argv[0]);
	par = CmdPar_ptr(NULL);
	CmdPar_load(par, par->name, 1);
	CmdPar_info(par, NULL);

	if	(CmdPar_eval(par, argc, argv, 0) <= 0)
		exit(EXIT_FAILURE);

	LogConfig(GetResource("Debug", NULL));
}

/*
Die Funktion |$1| setzt die Resource <name> auf den Wert <val>.
*/

void SetResource (const char *name, const char *val)
{
	CmdPar_setval(NULL, name, mstrcpy(val));
}

static char *vcopy (const char *val)
{
	size_t s, e;

	if	(val == NULL)	return NULL;
	if	(*val != '$')	return mstrcpy(val);

	s = 1;

	while (val[s] == ' ' || val[s] == '\t')
		s++;

	for (e = s + 1; val[e] != '$'; e++)
		if (val[e] == 0) return mstrcpy(val);

	do	e--;
	while	(val[e] == ' ' || val[e] == '\t');

	return (s <= e) ? mstrncpy(val + s, e - s + 1) : mstrcpy(val);
}

/*
Die Funktion |$1| setzt die Resource |Version| auf den Wert <val>.
Falls <val> mit einem |$| Zeichen beginnt und endet, werden diese
Begrenzer mit eventuell angrenzenden Leerzeichen entfernt.
*/

void SetVersion (const char *val)
{
	CmdPar_setval(NULL, "Version", vcopy(val));
}

/*
Die Funktion |$1| sucht die Resource <name> und liefert einen
Zeiger auf ihren Wert. Wurde |NULL| als Name übergeben, liefert
die Funktion einen Zeiger auf den Basisnamen des Kommandos.
Ist die Resource <name> nicht definiert, oder ist ihr Wert
ein NULL-String, liefert die Funktion den Zeiger <defval>.
*/

char *GetResource (const char *name, const char *defval)
{
	return CmdPar_getval(NULL, name, defval);
}

/*
Die Funktion |$1| sucht die Resource <name> und liefert das
Ganzzahläquivalent ihres Wertes. Die Konvertierung erfolgt
mit <|strtol(<ptr>, NULL, 0)|>. Ist die Resource <name> nicht definiert,
oder ist ihr Wert ein NULL-String, liefert die Funktion den Wert <defval>.
*/

int GetIntResource (const char *name, int defval)
{
	char *p = name ? CmdPar_getval(NULL, name, NULL) : NULL;
	return p ? strtol(p, NULL, 0) : defval;
}

/*
Die Funktion |$1| sucht die Resource <name> und liefert das
boolsche Äquivalent ihres Wertes.
Ist die Resource <name> nicht definiert,
oder ist ihr Wert ein NULL-String, liefert die Funktion den Wert 0.
Beginnt der Wert mit <"t"> oder <"T">, liefert die Funktion 1.
Beginnt der Wert mit einer Ziffer, liefert die Funktion das logische
äquivalent dieser Ziffer. Ansonsten liefert die Funktion 0.

Typische Werte für eine Flagresource sind:
<"0">, <"f">, <"false"> für 0 und <"1">, <"t">, <"true"> für 1.
*/

int GetFlagResource (const char *name)
{
	char *p = name ? CmdPar_getval(NULL, name, NULL) : NULL;

	if	(p == NULL)	return 0;
	if	(isdigit(*p))	return atoi(p) ? 1 : 0;
	if	(*p == 't' || *p == 'T')	return 1;

	return 0;
}

/*
$SeeAlso
\mref{CmdPar(1)}.\br
*/

