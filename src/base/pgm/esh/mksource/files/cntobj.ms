/*
Zählobjekt generieren

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

#ifndef	_MS_CNTOBJ
#define	_MS_CNTOBJ	1

#include <function.ms>

inline void CntObj(str name, Type_t type, IO def, str arg = NULL)
{
	arg = arg ? psub(arg) : "const void *ptr";

	fprintf(src, "\n#define\tCT_%s\t%#s\n", name, declare(type));
	
	Function f_set = Function("int set_" + name,
		arg ? arg : "const void *ptr");
	Function f_ini = Function("void ini_" + name,
		"char *ptr, size_t *idx, size_t dim");
	Function f_add = Function("void add_" + name, "char *ptr");
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
				typeof(expr(var)), offset(x, expr(var)));
		}
		else if	(var != ".")
		{
			expr(paste(" ", type, "x"));
			var = paste(".", "x", psub(var));
			fmt = sprintf("((%s *) (%%s + %d))[0]",
				typeof(expr(var)), offset(x, expr(var)));
		}
		else	fmt = sprintf("((%s *) (%%s))[0]", (str) type);

		setreg(1, sprintf(fmt, "ptr"));
		setreg(2, sprintf(fmt, "CB_" + name));

		if	(key == "return")
		{
			need_ret = false;
			f_set.line("return " + cmd + ";");
		}
		else if	(key == "set")
		{
			f_set.line(cmd + ";");
		}
		else if	(key == "ini")
		{
			need_ini = true;
			f_ini.line(cmd + ";");
		}
		else if	(key == "add")
		{
			f_add.line(cmd + ";");
		}
		else
		{
			f_ini.line("$1 = 0;");
			f_set.line("$2 = " + cmd + ";");
			f_add.line("$1 += $2;");
		}
	}

	if	(need_ret)
		f_set.line("return 1;");

	if	(need_ini)
		f_ini.write(false);
	else	fprintf(src, "#define\tini_%s\tNULL\n", name);

	fprintf(src, "\nstatic char *CB_%s[%d];\n", name, sizeof(type));
	f_set.write(false);
	f_add.write(false);
}

#endif	/* _MS_CNTOBJ */
