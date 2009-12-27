/*	Hilfsfunktionen zur Funktionsgenerierung
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_MS_FUNCTION
#define	_MS_FUNCTION	1

struct FunctionLine {
	int margin;	/* Rand */
	str line;	/* Zeilentext */
};

void FunctionLine::write()
{
	if	(strlen(line))
	{
		for (int i = 0; i < margin; i++)
			src << '\t';

		src << line;
	}

	src << '\n';
}

struct Function {
	str _name;
	str _arg;
	int depth;
	bool need_par;
	bool allow_par;
	DataBase lines;
};

restricted bool Function ()
	return (bool) _name

str Function()	
	return _name ? sprintf("%s(%s)", _name, _arg ? _arg : "void") : NULL;

Function Function(str _name)
	return { psub(_name), NULL, 1, false, false, DataBase(FunctionLine) };

inline Function Function(str _name, str _args, ...)
{
	_name = psub(_name, va_list);
	return { _name, psub(_args), 1, false, false, DataBase(FunctionLine) };
}

void Function::write(bool global = false)
{
	if	(!_name)	return;

	str fname = sprintf("%s (%s)", _name, _arg ? _arg : "void");

	if	(global)
	{
		hdr << "extern " << fname << ";\n";
	}
	else	fname = "static " + fname;

	src << "\n";
	src << fname;
	src << "\n{\n";

	for (x in lines)
		x.write();

	src << "}\n\n";
}

void Function::newpar & (void)
	need_par = allow_par;

void Function::ignorepar & (void)
	need_par = allow_par = false;

void Function::startline & (void)
{
	if	(need_par)
	{
		lines += { 0, NULL };
		need_par = false;
	}

	allow_par = true;
}

void Function::add_line & (int dp, str cmd)
{
	if	(strlen(cmd))
	{
		startline();
		lines += { dp, cmd };
	}
	else	newpar();
}


void Function::beg & (str fmt = NULL)
	startline(), lines += { depth++, psub(fmt) + "{" }, ignorepar();

void Function::end & (str fmt = NULL)
	lines += { --depth, "}" + psub(fmt) }, newpar();

void Function::line & (str fmt = NULL, ...)
	add_line(depth, psub(fmt, va_list));

void Function::ppline & (str fmt = NULL, ...)
	add_line(0, psub(fmt, va_list));

inline void Function::comment & (str fmt = NULL, ...)
{
	startline();
	lines += { 0, "/*\t" + psub(fmt, va_list) };
	lines += { 0, "*/" };
	newpar();
}

void Function::fline & (str fmt, ...)
	add_line(depth, sprintf(fmt, va_list));

/*	Switch - Anweisungen
*/

void Function::switch & (str expr, ...)
	line("switch (" + expr + ")", va_list), beg();

inline void Function::case & (str key, str _cmd = NULL, ...)
{
	_cmd = psub(_cmd, va_list);
	key = key ? "case " + psub(key) : "default";
	add_line(depth - 1, paste("\t", key + ":", _cmd));
}

inline void Function::cblock & (str key, str _cmd, ...)
{
	_cmd = psub(_cmd, va_list);
	key = key ? "case " + psub(key) : "default";

	add_line(depth - 1, key + ":");

	for (x in split(_cmd, "\n"))
		add_line(depth, x);
}


inline void Function::body & (str fmt = NULL, ...)
	for (x in split(psub(fmt, va_list), "\n"))
		add_line(depth, x);

void Function::byte_order_dep & (str fmt, str std, str rev, ...)
{
	ppline("#if\tREVBYTEORDER");
	setreg(0, rev);
	body(fmt, va_list);
	ppline("#else");
	setreg(0, std);
	body(fmt, va_list)
	ppline("#endif");
}

void Function::block & (str head, str body, ...)
{
	startline();

	if	(head)
		lines += { depth, psub(head, va_list) };

	lines += { depth++, "{" };

	for (x in split(psub(body), "\n"))
		lines += { depth, x };

	lines += { --depth, "}" };
	newpar();
}

int Function::pfbody & (str fname, IO def, bool in_switch = false)
{
	str key, fmt;
	bool need_break = false;
	bool parflag = false;

	while (getline(def, key, fmt))
	{
		if	(parflag)
		{
			newpar();
			parflag = false;
		}

		key = psub(key);

		if	(key[0] == "@")
		{
			key = substr(key, 1);

			if	(key == "if")
			{
				newpar();
				line("if\t(" + fmt + ")");
				beg();
				pfbody(fname, def);
				parflag = true;
			}
			else if	(key == "elif")
			{
				end();
				line("else if\t(" + fmt + ")");
				beg();
			}
			else if	(key == "switch")
			{
				line();
				line("switch (" + fmt + ")");
				beg();
				pfbody(fname, def, true);
				parflag = true;
			}
			else if	(key == "else")
			{
				end();
				line("else");
				beg();
			}
			else if	(key == "case")
			{
				if	(need_break) line("break;");

				case(fmt);
				need_break = false;
				continue;
			}
			else if	(key == "default")
			{
				if	(need_break) line("break;");

				case(NULL);
				need_break = false;
				continue;
			}
			else if	(key == "end")
			{
				if	(need_break) line("break;");

				in_switch = false;
				end();
				return;
			}

			continue;
		}
		else if	(fmt == NULL)
		{
			line();
			continue;
		}
		else if	(fmt[0] == '$')
		{
			fline("io_puts(%s, %s);", psub(substr(fmt, 1), key), fname);
		}
		else if	(fmt[0] == '"')
		{
			fline("io_puts(%s, %s);", fmt, fname);
		}
		else if	(fmt[0] == '\'')
		{
			fline("io_putc(%s, %s);", fmt, fname);
		}
		else if	(fmt[0] == '@')
		{
			fline("io_puts(%#s, %s);", substr(fmt, 1), fname);
		}
		else if	(fmt[0] == '#')
		{
			fline("io_printf(%s, %#s, %s);", fname,
				substr(fmt, 1), key);
		}
		else if	(fmt[0] == '?')
		{
			int n = 1;

			while (fmt[n] == ' ')
				n++;

			fmt = substr(fmt, n);

			if	(fmt[0] == '\'')
				fline("io_putc(%s ? %s, %s);", key, fmt, fname);
			else if	(fmt[0] == '"')
				fline("io_puts(%s ? %s, %s);", key, fmt, fname);
			else
			{
				str v1 = strcut(fmt, ":");
				fline("io_printf(%s, %s ? %#s : %#s, %s);",
					fname, key, v1, fmt, key);
			}
		}
		else if	(fmt[0] == '=')
		{
			int n = 1;
			str k = "default";
			line();
			line("switch ($1)", key);
			line("{");

			for (z in split(substr(fmt, 1), ","))
			{
				fline("%s:\tio_puts(%#s, %s); break;",
					k, z, fname);
				k = sprintf("case %2d", n++);
			}

			line("}");
			line();
		}
		else if	(fmt[0] == '\\')
		{
			line(substr(fmt, 1) + ";", key, fname);
		}
		else	line(fmt + ";", key, fname);

		need_break = in_switch;
	}

	return parflag;
}

/*	Einfache Funktionsdefinitionen
*/

inline void LocalFunction (str _name, str _args, str _body, ...)
{
	Function f = Function(_name, _args, va_list);
	f.body(_body);
	f.write(false);
}

inline void GlobalFunction (str _name, str _args, str _body, ...)
{
	Function f = Function(_name, _args, va_list);
	f.body(_body);
	f.write(true);
}

#endif	/* _MS_FUNCTION */
