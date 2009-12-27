/*
Kommandoparameterwerte setzen/abfragen

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

#include <EFEU/CmdPar.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>

/**/
static int var_cmp (const void *a, const void *b)
{
	const CmdParVar *va = a;
	const CmdParVar *vb = b;
	return mstrcmp(va->name, vb->name);
}

/*
Die Funktion |$1| ruft die Variablendefinition <name> der
Kommandoparameterstruktur ab. Falls <flag> verschieden von 0 ist,
wird sie bei Bedarf eingerichtet.
Bei einem Nullpointer als Argument liefert die Funktion einen Nullpointer.
Die Komponente |name| darf nicht verändert werden, da sie als Sortierschlüssel
dient!
*/

CmdParVar *CmdPar_var (CmdPar *par, const char *name, int flag)
{
	CmdParVar var, *ptr;

	if	(name == NULL)	return NULL;

	par = CmdPar_ptr(par);
	var.name = (char *) name;
	var.value = NULL;
	var.desc = NULL;
	ptr = vb_search(&par->var, &var, var_cmp, VB_SEARCH);

	if	(ptr || flag == 0)	return ptr;

	var.name = mstrcpy(name);
	return vb_search(&par->var, &var, var_cmp, VB_ENTER);
}

/*
Die Funktion |$1| ruft den Wert der Variablen <name> der
Kommandoparameterstruktur ab. Falls für <name>
ein Nullpointer angegeben wurde, wird der Aufrufname geliefert.
Ist der Wert der abgerufenen Variablen NULL, wird er
durch <defval> ersetzt. Die von |$1| zurückgegebene Zeichenkette
darf nicht verändert werden!
*/

char *CmdPar_getval (CmdPar *par, const char *name, const char *defval)
{
	if	(name)
	{
		CmdParVar *var = CmdPar_var(par, name, 0);

		if	(var && var->value)	return var->value;
	}
	else if	(par && par->name)
	{
		return par->name;
	}

	return (char *) defval;
}


/*
Die Funktion |$1| setzt den Wert der Variablen <name> der
Kommandoparameterstruktur auf <value>. Falls für <name>
ein Nullpointer angegeben wurde, wird der Aufrufname geändert.
Für <value> sollte nur ein dynamisch generierter
String übergeben werden (Vergleiche dazu \mref{mstring(3)}).
*/

void CmdPar_setval (CmdPar *par, const char *name, char *value)
{
	CmdParVar *var;

	par = CmdPar_ptr(par);
	var = CmdPar_var(par, name, 1);
	
	if	(var)
	{
		memfree(var->value);
		var->value = value;
	}
	else
	{
		memfree(par->name);
		par->name = value;
	}
}

/*
Die Funktion |$1| gibt alle Parameterwerte in der
Kommandoparameterstruktur nach <io> aus. Die Ausgabe erfolgt
mithilfe von |io_psub| und Format <fmt>.
Dabei wird der Parametername (Register 1)
und der Parameterwert (Register 2) mit Parametersubstitution eingesetz.
Wird als Formatangabe ein Nullpointer angegeben, erfolgt eine
Standardausgabe der Form <|<name>="<value>"|>.
*/

void CmdPar_showval (CmdPar *par, IO *io, const char *fmt)
{
	CmdParVar *var;
	size_t n;

	par = CmdPar_ptr(par);

	for (n = par->var.used, var = par->var.data; n-- > 0; var++)
	{
		if	(fmt)
		{
			io_psubarg(io, fmt, "nss", var->name, var->value);
		}
		else	io_printf(io, "%s=%#s\n", var->name, var->value);
	}
}


/*
$SeeAlso
\mref{CmdPar(3)},
\mref{CmdParCall(3)},
\mref{CmdParDef(3)},
\mref{CmdParEval(3)},
\mref{CmdParExpand(3)},
\mref{CmdParKey(3)},
\mref{CmdPar_eval(3)},
\mref{CmdPar_list(3)},
\mref{CmdPar_load(3)},
\mref{CmdPar_psub(3)},
\mref{CmdPar_usage(3)},
\mref{CmdPar_write(3)},
\mref{Resource(3)},
\mref{CmdPar(7)}.
*/

