/*
:*:create index functions
:de:Konstruktion von Indexfunktionen

$Header	<$1>
$Copyright (C) 1999 Erich Frühstück
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

#ifndef	index_ms
#define	index_ms	1

#include <function.ms>
#include <label.ms>

/*
Der Datentyp |$1| dient zur Generierung einer Indexfunktion und
einer zugehörigen Indextabelle.
Indexfunktionen werden primär zur Konstruktion von Zählklassen
verwendet.
*/

struct IndexFunc {
	Function func;
	Label lbl;
	str keysep;
	str lblsep;
	bool mzero;	/* 0 als missing value */
};

/*
Beim Konstruktor |$1| gibt <_name> den Namen
und <_args> die Argumentliste der Indexfunktion an.
Auf <_name> und <_args> wird eine Parametersubstitution angewendet.
Alle weiteren Argumente setzen Stringregister,
die bei der Parametersubstitution zur Verfügung stehen.
*/

IndexFunc IndexFunc (str _name, str _args, ...)
	return { Function("int " + _name + "_idx", _args, va_list),
		Label(_name), "_", ", ", false }

/*
Mit Hilfe der Funktion |$1| wird die Indexfunktion und die Indextabelle
ausgegeben. Das Argument <glob> bestimmt, ob die Funktion und die Tabelle
global oder statisch eingerichtet werden soll.
*/

void IndexFunc::write (bool glob = true)
{
	func.write(glob);
	lbl.tab(glob);
}

static void IndexFunc::line (str fmt = NULL, ...)
	func.line(fmt, va_list)

static void IndexFunc::fline (str fmt = NULL, ...)
	func.fline(fmt, va_list)

/*
Die Funktion |$1| generiert den Funktionsrumpf der Indexfunktion
und die zugehörigen Einträge in die Indextabelle. Die einzelnen
Zeilen der Definitionsstruktur <_io> bestehen aus Steueranweisungen
und Indexeinträgen.
*/

inline void IndexFunc::body(IO _io, ...)
{
	str _cmd;
	bool rval = false;

	while (getline(_io, _cmd))
	{
		if	(_cmd[0] == '@')
			rval = subfunc(_io, _cmd);
		else	func.line(_cmd, va_list);
	}

	if	(rval)	;
	else if	(mzero)	func.fline("return 0;");
	else		func.fline("return %d;", lbl.entry.dim);
}

/*
Der Datentyp |$1| wird intern für den aktuellen Status der
Indexgenerierung verwendet.
*/

struct IndexPar {
	int val;
	int mode;
	str name;
	str desc;
	str last;
	str expr;
};

void IndexPar::operator+= (str fmt)
	expr = paste(" ||\n\t  ", expr, fmt);

inline int IndexFunc::beg (IndexPar &par, str key, str label, IndexPar base)
{
	par.val = lbl.entry.dim + mzero;
	str name = xstrcut(label, "%s");

	if	(name)
	{
		str last = par.last;
		par.name = paste(keysep, base.name, psub(name));
		par.desc = paste(lblsep, base.desc, psub(label ? label : name));
		lbl.add(par.name, par.desc);
		par.last = key;
	}
	else	par.desc = NULL;

	par.mode = 1;
	return par.val;
}

inline void IndexFunc::cond (IndexPar &par)
{
	if	(par.expr && par.mode == 1)
	{
		func.newpar();
		func.body("if\t( " + par.expr + " )");
		par.expr = NULL;
	}
}

inline void IndexFunc::block (IndexPar &par)
{
	if	(par.mode == 1)
	{
		cond(par);
		func.beg();
		par.mode = 2;
	}
	else	func.newpar();
}

inline void IndexFunc::end (IndexPar &par, str fmt = "return %d;")
{
	if	(par.mode == 0)	return;

	cond(par);

	if	(fmt == NULL)
	{
		;
	}
	else if	(par.name)
	{
		func.fline(fmt + " /* %s */", par.val, par.name);
		par.name = NULL;
	}
	else	func.fline(fmt, par.val);

	if	(par.mode == 2)	func.end();

	par.mode = 0;
}

/*
Die Funktion |$1| testet die Steueranweisung |cmd| und ruft die
entsprechende Teilfunktion auf.
*/

inline bool IndexFunc::subfunc (IO _io, str cmd, IndexPar base = NULL)
{
	if	(!cmd || cmd[0] != '@')	return false;

	str key = xstrcut(cmd, "%s");

	if	(key == "@test")
		return this.test(xstrcut(cmd, NULL), _io, base);
	else if	(key == "@switch")
		return this.switch(xstrcut(cmd, "%s"), cmd, _io, base);
	else if	(key == "@default")
		return this.defval(cmd, base);
	else if	(key == "@range")
		return this.range(cmd, base);
	else if	(key == "@eval")
		eval(cmd);

	return false;
}


/*
\ttitem	$1
	Standardeintrag
*/

inline bool IndexFunc::defval (str label, IndexPar base = NULL)
{
	IndexPar par;
	beg(par, NULL, label, base);
	end(par, "return %d;");
	return true;
}

/*
\ttitem	$1
	Bereichstest
*/

inline bool IndexFunc::range (str & label, IndexPar base = NULL)
{
	str expr = xstrcut(label, "%s");
	int first = xstrcut(label, "%s");
	int last = xstrcut(label, "%s");
	str fmt = label ? xstrcut(label, "%s") : "$(n)";

	func.beg();
	func.line("int x = (" + expr + psub(") - $(first);"));
	func.line();
	func.line("if (x < 0)	x = 0;");
	func.line("if (x > $1)	x = $1;", last - first);
	func.line();

	if	(!base.mode)
	{
		base.val = lbl.entry.dim + mzero;
		base.mode = 1;
	}
	else	first++;

	for (int n = first; n <= last; n++)
	{
		str name = paste(keysep, base.name, psub(fmt));
		str desc = paste(lblsep, base.desc, psub(label ? label : fmt));
		lbl.add(name, desc);
	}
		
	end(base, "return x + %d;");
	func.end();
	return true;
}


/*
\ttitem	$1
	Testblock
*/

inline bool IndexFunc::test (str fmt, IO _io, IndexPar base = NULL)
{
	str key, cmd;
	bool retval = false;
	bool flag = false;
	int else_val = 0;
	str else_name = NULL;
	int val = 0;
	str end_fmt = "\treturn %d;";
	str test_func = NULL;
	IndexPar par;
	List_t list;

	while (getline(_io, cmd))
	{
		if	(cmd == "@end")	break;

		if	(cmd[0] == '@')
		{
			if	(listcmp("%n", cmd[1]))
			{
				block(par);
				end_fmt = "return %d;";

				if	(subfunc(_io, cmd, par))
					end(par, NULL);

				flag = true;
				continue;
			}
			else	cmd = substr(cmd, 1);
		}

		key = psub(xstrcut(cmd, "%s"));

		if	(key[0] == '(')
		{
			list = expr(key);
		}
		else	list = { key };

		for (z in list)
		{
			if	(flag || par.name)
				end(par, end_fmt);

			flag = false;
			key = z;

			if	(key == "*")
			{
				else_val = beg(par, NULL, cmd, base);
				else_name = par.name;
				end(par, NULL);
				retval = true;
			}
			else
			{
				beg(par, key, cmd, base);
				par += (fmt ? psub(fmt) : key);
				end_fmt = "\treturn %d;";
			}
		}
	}

	end(par, end_fmt);

	if	(retval)
	{
		func.newpar();
		func.fline("return %d; /* %s */", else_val, else_name);
	}

	return retval;
}


/*
\ttitem $1
	Switch-Block
*/


inline bool IndexFunc::switch (str expr, str fmt, IO _io, IndexPar base = NULL)
{
	str key, _cmd;
	bool add_entry = false;
	bool retval = false;
	IndexPar par;

	func.line();
	func.line("switch (" + expr + ")");
	func.beg();

	while (getline(_io, _cmd))
	{
		if	(_cmd == "@end")	break;

		if	(_cmd[0] == '@')
		{
			if	(listcmp("%n", _cmd[1]))
			{
				if	(subfunc(_io, _cmd, par))
					end(par, NULL);

				continue;
			}
			else	_cmd = substr(_cmd, 1);
		}

		if	(par.name)
			end(par, "return %d;");

		key = xstrcut(_cmd, "%s");

		if	(key == "*")
		{
			key = NULL;
			func.case(NULL);
			retval = true;
		}
		else	func.case(fmt ? psub(fmt) : key);

		beg(par, key, _cmd, base);
	}

	if	(par.mode && !par.name)
	{
		end(par, "break;");
		retval = false;
	}
	else	end(par, "return %d;");

	func.end();
	return retval;
}

/*
Standardisierte Indexfunktionen können mit den Funktionen
|GlobalIndexFunc| und |LocalIndexFunc| konstruiert werden.
Das Argument <_body> bestimmt den Funktionsrumpf, der mit
|IndexFunc::body| aufbereitet wird.
Für alle anderen Argumente gilt das bei der Beschreibung
des Konstruktors gesagte.
*/

inline void GlobalIndexFunc (str _name, str _args, str _body, ...)
{
	IndexFunc f = IndexFunc(_name, _args, va_list);
	f.body(_body, va_list);
	f.write(true);
}

inline void LocalIndexFunc (str _name, str _args, str _body, ...)
{
	IndexFunc f = IndexFunc(_name, _args, va_list);
	f.body(_body, va_list);
	f.write(false);
}

/*
$Examples
Das Beispiel zeigt die Konstruktion einer Indexfunktion für das
Geschlecht.

\ms_example	index
config hdr src

#include <index.ms>

LocalIndexFunc("geschl", "const char *key", string !
@switch key[0]
*	UBKG	Unbekanntes Geschlecht
'M'	MANN	Männer
'F'	FRAU	Frauen
@end
!);

\end

$SeeAlso
\mref{mksource(1)}, \mref{cntdef(7)}, \mref{function(7)}, \mref{label(7)}.
*/

#endif	/* index.ms */
