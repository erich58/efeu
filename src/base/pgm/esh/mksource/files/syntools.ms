/*	Synthesis-Datenbankkonverter
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#ifndef	_syntools_ms
#define	_syntools_ms	1

#include <enum.ms>
#include <macro.ms>
#include <struct.ms>
#include <var.ms>
#include <function.ms>
#include <vstat.ms>

void SynInclude (IO io, ...)
{
	for (x in va_list)
		fprintf(io, "#include <Synthesis/%s.h>\n", x);

	io << '\n';
}

void InfoInclude (IO io, ...)
{
	for (x in va_list)
		fprintf(io, "#include <%s.info>\n", x);

	io << '\n';
}

/*	Parameterdefinitionen
*/

void KonvPar (str name, str def,
	str init = "return memalloc(sizeof($1_t));",
	str clean = "memfree(par);", bool glob = false)
{
	Function f;

	setreg(1, psub(name));

	if	(def)
		Struct("$1", NULL, def, glob);

	f = Function("void *$1_init", "const char *arg");
	f.body(init);
	f.write(glob);

	f = Function("void $1_clean", "$1_t *par");
	f.body(clean);
	f.write(glob);
}

void TransPar (str name, str def,
	str init = "return memalloc(sizeof($1_t));",
	str clean = "memfree(par);", bool glob = false)
{
	Function f;

	setreg(1, psub(name));

	if	(def)
		Struct("$1", NULL, def, glob);

	f = Function("void *$1_init", "const char *opt, const char *arg");
	f.body(init);
	f.write(glob);

	f = Function("void $1_clean", "io_t *out, $1_t *par");
	f.body(clean);
	f.write(glob);
}

inline str SynInfo(str _name, str _desc)
{
	if	(!_desc)	return "NULL";

	setreg(1, psub(_name));
	fpsub(src, "\nstatic InfoString_t $1_desc = InfoString(\"\\\\\n");
	src.mputs(strsub(psub(_desc), "\n", "\\n\n"), "\"\n");
	src << "\");\n\n";
	return psub("(Info_t *) &$1_desc");
}

inline void Function::compline (str name, str fmt = "$1->$2")
{
	str a, b;

	if	(name[0] == '-')
	{
		name = substr(name, 1);
		a = psub(fmt, "b", name);
		b = psub(fmt, "a", name);
	}
	else
	{
		if	(name[0] == '+')
			name = substr(name, 1);

		a = psub(fmt, "a", name);
		b = psub(fmt, "b", name);
	}

	line("if\t($1 < $2)\treturn -1;", a, b);
	line("if\t($1 > $2)\treturn 1;");
}

inline void CompFunc (str _name, str _type, IO _def, bool glob = false)
{
	_type = psub(_type);
	Function f = Function("int " + _name,
		"const $1_t *a, const $1_t *b", _type);

	str key, cmd;
	bool need_return = true;

	while (getline(_def, key, cmd))
	{
		if	(key == "line")
		{
			f.line(cmd);
		}
		else if	(key == "var")
		{
			str type = strcut(cmd, "%s");
			f.line("$1 a_$2, b_$2;", type, cmd);
		}
		else if	(key == "sub")
		{
			f.beg();
			f.line("register int n = " + cmd + ";", _type);
			f.line();
			f.line("if\t(n)\treturn n;");
			f.end();
		}
		else if	(key == "return")
		{
			f.line("return " + cmd + ";", _type);
			need_return = false;
		}
		else if	(key == "expr")
		{
			str name = strcut(cmd, "%s");
			str var;

			if	(name[0] == '-' || name[0] == '+')
			{
				var = substr(name, 1);
			}
			else	var = name;

			f.line("$2 = " + cmd + ";", "a", "a_" + var);
			f.line("$2 = " + cmd + ";", "b", "b_" + var);
			f.compline(name, "$1_$2");
		}
		else if	(key == "macro")
		{
			for (x in split(cmd, "%s,"))
				if (strlen(x)) f.compline(x, "$2($1)");
		}
		else
		{
			for (x in split(cmd, "%s,"))
				if (strlen(x)) f.compline(x, "$1->$2");
		}
	}

	if	(need_return)
		f.line("return 0;");

	f.write(glob);
}

inline void Function::testline (str name, str fmt = "$1->$2", bool flag)
{
	str expr = sprintf("if\t($1 $3 $2)\treturn %d;", flag ? 0 : 1);

	if	(name[0] == '!')
	{
		name = substr(name, 1);
		flag = !flag;
	}

	line(expr, psub(fmt, "a", name), psub(fmt, "b", name),
		flag ? "==" : "!=");
}

inline void TestFunc (str _name, str _type, IO _def, bool glob = false)
{
	_type = psub(_type);
	Function f = Function("int " + _name,
		"const $1_t *a, const $1_t *b", _type);

	str key, cmd;
	bool mode;

	while (getline(_def, key, cmd))
	{
		if	(key[0] == '!')
		{
			key = substr(key, 1);
			mode = true;
		}
		else	mode = false;

		if	(key == "line")
		{
			f.line(cmd);
		}
		else if	(key == "var")
		{
			str type = strcut(cmd, "%s");
			f.line("$1 a_$2, b_$2;", type, cmd);
		}
		else if	(key == "expr")
		{
			str name = strcut(cmd, "%s");
			f.line("$2 = " + cmd + ";", "a", "a_" + name);
			f.line("$2 = " + cmd + ";", "b", "b_" + name);
			f.testline(name, "$1_$2", mode);
		}
		else if	(key == "macro")
		{
			for (x in split(cmd, "%s,"))
				if (strlen(x)) f.testline(x, "$2($1)", mode);
		}
		else
		{
			for (x in split(cmd, "%s,"))
				if (strlen(x)) f.testline(x, "$1->$2", mode);
		}
	}

	f.fline("return 0;");
	f.write(glob);
}

#endif	/* syntools.ms */
