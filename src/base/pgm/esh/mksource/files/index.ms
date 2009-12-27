/*
Indexfunktion

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

struct IndexFunc {
	Function func;
	Label lbl;
	str keysep;
	str lblsep;
};


IndexFunc IndexFunc (str name, str args, ...)
	return { Function("int " + name + "_idx", args, va_list),
		Label(name), "_", ", " }

void IndexFunc::write (bool glob = true)
{
	func.write(glob);
	lbl.tab(glob);
}

void IndexFunc::line (str fmt = NULL, ...)
	func.line(fmt, va_list)

void IndexFunc::fline (str fmt = NULL, ...)
	func.fline(fmt, va_list)

inline void IndexFunc::body(IO _io)
{
	str _cmd;
	bool rval = false;

	while (getline(_io, _cmd))
	{
		if	(_cmd[0] == '@')
			rval = subfunc(_io, _cmd);
		else	func.line(_cmd);
	}

	if	(!rval)
		func.fline("return %d;", dim(lbl));
}

/*	Hilfsfunktionen zur Konstruktion von Einträgen
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
	par.val = dim(lbl);
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

/*	Kontrollfunktion
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
	else if	(key == "@eval")
		eval(cmd);

	return false;
}


/*	Standardeintrag
*/

inline bool IndexFunc::defval (str label, IndexPar base = NULL)
{
	IndexPar par;
	beg(par, NULL, label, base);
	end(par, "return %d;");
	return true;
}


/*	Testblock
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


/*	Switch-Block
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

#endif	/* index.ms */
