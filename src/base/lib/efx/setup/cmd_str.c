/*
Zeichen und Strings

$Copyright (C) 1994, 2001 Erich Frühstück
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

#include <EFEU/object.h>
#include <EFEU/cmdconfig.h>
#include <ctype.h>

#define	RVSTR	Val_str(rval)
#define	CHAR(n)	Val_char(arg[n])
#define	STR(n)	Val_str(arg[n])
#define	INT(n)	Val_int(arg[n])

#define	EXPR(name, expr)	\
CEXPR(name, Val_str(rval) = expr)

#define	COMPARE(name, op)	\
CEXPR(name, Val_int(rval) = mstrcmp(STR(0), STR(1)) op 0)


/*	Konvertierungsfunktionen
*/

CEXPR(k_char2int, Val_int(rval) = CHAR(0))
CEXPR(k_int2char, Val_char(rval) = (uchar_t) Val_int(arg[0]))
CEXPR(k_long2char, Val_char(rval) = (uchar_t) Val_long(arg[0]))

CEXPR(k_str2int, Val_int(rval) = STR(0) ? (int) strtol(STR(0), NULL, 0) : 0)
CEXPR(k_str2long, Val_long(rval) = STR(0) ? strtol(STR(0), NULL, 0) : 0)
CEXPR(c_str2int, Val_int(rval) = STR(0) ?
	(int) strtol(STR(0), NULL, Val_int(arg[1])) : 0)
CEXPR(c_str2long, Val_long(rval) = STR(0) ?
	strtol(STR(0), NULL, Val_int(arg[1])) : 0)
CEXPR(k_str2double, Val_double(rval) = STR(0) ? strtod(STR(0), NULL) : 0.)

EXPR(k_ptr2str, NULL)
EXPR(k_char2str, msprintf("%c", Val_char(arg[0])))
EXPR(k_int2str, msprintf("%i", Val_int(arg[0])))
EXPR(k_long2str, msprintf("%li", Val_long(arg[0])))
EXPR(k_double2str, msprintf("%g", Val_double(arg[0])))


COMPARE(b_str_lt, <)
COMPARE(b_str_le, <=)
COMPARE(b_str_eq, ==)
COMPARE(b_str_ne, !=)
COMPARE(b_str_ge, >=)
COMPARE(b_str_gt, >)

static char *strmul (const char *str, int n)
{
	strbuf_t *sb;

	if	(!str || n <= 0)	return NULL;

	sb = new_strbuf(0);

	for (; n > 0; n--)
		sb_puts(str, sb);

	return sb2str(sb);
}

EXPR(f_str_lmul, strmul(STR(1), INT(0)))
EXPR(f_str_rmul, strmul(STR(0), INT(1)))

EXPR(f_str_add,	mstrpaste(NULL, STR(0), STR(1)))
EXPR(f_strpaste, mstrpaste(STR(0), STR(1), STR(2)))
EXPR(f_langcpy, mlangcpy(STR(0), STR(1)))
CEXPR(f_strlen, Val_int(rval) = STR(0) ? strlen(STR(0)) : 0)

/*	Spezialfunktionen
*/

static void k_nchar2str (Func_t *func, void *rval, void **arg)
{
	int i, n, c;
	char *s;

	c = Val_char(arg[0]);
	n = Val_int(arg[1]);
	n = max(0, n);
	s = memalloc(n + 1);

	for (i = 0; i < n; i++)
		s[i] = c;

	s[i] = 0;
	Val_str(rval) = s;
}

static void k_type2str (Func_t *func, void *rval, void **arg)
{
	Type_t *type = Val_type(arg[0]);
	Val_str(rval) = type ? mstrcpy(type->name) : NULL;
}

static void f_str_index (Func_t *func, void *rval, void **arg)
{
	register char *s = STR(0);
	register int i = Val_int(arg[1]);
	Val_char(rval) = (s && i < strlen(s)) ? s[i] : 0;
}

static void f_substr (Func_t *func, void *rval, void **arg)
{
	char *s;
	int pos, len;

	s = STR(0);
	pos = Val_int(arg[1]);
	len = Val_int(arg[2]);

	if	(s == NULL || strlen(s) < pos)
		RVSTR = NULL;
	else if	(len != 0)
		RVSTR = mstrncpy(s + pos, len);
	else	RVSTR = mstrcpy(s + pos);
}

static void f_strcut (Func_t *func, void *rval, void **arg)
{
	char *s;
	char *p;
	int i;

	if	((s = STR(0)) == NULL)
	{
		RVSTR = NULL;
		return;
	}

	p = STR(1);

	for (i = 0; s[i] != 0; i++)
	{
		if	(!listcmp(p, s[i]))	continue;

		RVSTR = i ? mstrncpy(s, i) : NULL;
		i++;

		if	(INT(2))
			while (s[i] && listcmp(p, s[i]))
				i++;

		STR(0) = s[i] ? mstrcpy(s + i) : NULL;
		memfree(s);
		return;
	}

	RVSTR = s;
	STR(0) = NULL;
}

static void f_xstrcut (Func_t *func, void *rval, void **arg)
{
	strbuf_t *sb;
	char *delim, *base;
	io_t *in, *out;
	int c;

	if	((base = STR(0)) == NULL)
	{
		RVSTR = NULL;
		return;
	}

	in = io_cstr(base);
	delim = STR(1);
	sb = new_strbuf(0);
	out = io_strbuf(sb);

	while ((c = io_getc(in)) != EOF)
	{
		if	(c == '"')
		{
			io_xcopy(in, out, "\"");
			io_getc(in);
		}
		else if	(c == '\'')
		{
			io_mcopy(in, out, "\'");
			io_getc(in);
		}
		else if	(c == '\\')
		{
			c = io_getc(in);

			if	(c == EOF)	c = '\\';

			io_putc(c, out);
		}
		else if	(listcmp(delim, c))
		{
			break;
		}
		else	io_putc(c, out);
	}

	io_close(out);
	RVSTR = sb->pos ? mstrncpy((char *) sb->data, sb->pos) : NULL;
	sb->pos = 0;
	sb_sync(sb);

/*	Reststring
*/
	if	(INT(2))
		io_eat(in, delim);

	while ((c = io_getc(in)) != EOF)
		sb_putc(c, sb);

	STR(0) = sb2str(sb);
	io_close(in);
	memfree(base);
}

static void f_strsub (Func_t *func, void *rval, void **arg)
{
	char *s, *p;
	char *repl;
	char *in;
	int flag;
	strbuf_t *sb;

	if	((s = STR(0)) == NULL)
	{
		RVSTR = NULL;
		return;
	}

	repl = STR(1);
	in = STR(2);
	flag = ! Val_bool(arg[3]);
	sb = new_strbuf(0);

	while (*s != 0)
	{
		if	(patcmp(repl, s, &p))
		{
			sb_puts(in, sb);

			if	(flag)
			{
				sb_puts(p, sb);
				break;
			}
			else if	(s == p)
			{
				s++;
			}
			else	s = p;
		}
		else
		{
			sb_putc(*s, sb);
			s++;
		}
	}

	RVSTR = sb2str(sb);
}

static void f_upper (Func_t *func, void *rval, void **arg)
{
	if	((RVSTR = mstrcpy(STR(0))) != NULL)
	{
		char *s;

		for (s = RVSTR; *s != 0; s++)
		{
			switch (*s)
			{
			case 'ä':	*s = 'Ä'; break;
			case 'ö':	*s = 'Ö'; break;
			case 'ü':	*s = 'Ü'; break;
			default:	*s = toupper(*s); break;
			}
		}
	}
}

static void f_lower (Func_t *func, void *rval, void **arg)
{
	if	((RVSTR = mstrcpy(STR(0))) != NULL)
	{
		char *s;

		for (s = RVSTR; *s != 0; s++)
		{
			switch (*s)
			{
			case 'Ä':	*s = 'ä'; break;
			case 'Ö':	*s = 'ö'; break;
			case 'Ü':	*s = 'ü'; break;
			default:	*s = tolower(*s); break;
			}
		}
	}
}

static void f_lexsortkey (Func_t *func, void *rval, void **arg)
{
	RVSTR = mstrcpy(lexsortkey(Val_str(arg[0]), NULL));
}

static FuncDef_t func_str[] = {
	{ FUNC_PROMOTION, &Type_int, "char ()", k_char2int },
	{ 0, &Type_char, "char (int)", k_int2char },
	{ 0, &Type_char, "char (long)", k_long2char },

	{ 0, &Type_int, "int (str)", k_str2int },
	{ 0, &Type_long, "long (str)", k_str2long },
	{ 0, &Type_int, "int (str, int base)", c_str2int },
	{ 0, &Type_long, "long (str, int base)", c_str2long },
	{ 0, &Type_double, "double (str)", k_str2double },
	{ 0, &Type_str, "char ()", k_char2str },
	{ 0, &Type_str, "str (int)", k_int2str },
	{ 0, &Type_str, "str (long)", k_long2str },
	{ 0, &Type_str, "str (double)", k_double2str },
	{ 0, &Type_str, "str (char, int = 1)", k_nchar2str },
	{ 0, &Type_str, "str (Type_t)", k_type2str },
	{ 0, &Type_str, "str (_Ptr_)", k_ptr2str },

	{ FUNC_VIRTUAL, &Type_char, "operator[] (str, int)", f_str_index },
	{ FUNC_VIRTUAL, &Type_str, "operator+ (str, str)", f_str_add },
	{ FUNC_VIRTUAL, &Type_str, "operator* (int, str)", f_str_lmul },
	{ FUNC_VIRTUAL, &Type_str, "operator* (str, int)", f_str_rmul },
	{ FUNC_VIRTUAL, &Type_bool, "operator< (str, str)", b_str_lt },
	{ FUNC_VIRTUAL, &Type_bool, "operator<= (str, str)", b_str_le },
	{ FUNC_VIRTUAL, &Type_bool, "operator== (str, str)", b_str_eq },
	{ FUNC_VIRTUAL, &Type_bool, "operator!= (str, str)", b_str_ne },
	{ FUNC_VIRTUAL, &Type_bool, "operator>= (str, str)", b_str_ge },
	{ FUNC_VIRTUAL, &Type_bool, "operator> (str, str)", b_str_gt },

	{ 0, &Type_str, "substr (str s, int pos, int len = 0)", f_substr },
	{ 0, &Type_str, "strcut (str & s, str delim, bool flag = true)",
		f_strcut },
	{ 0, &Type_str, "xstrcut (str & s, str delim, bool flag = true)",
		f_xstrcut },
	{ 0, &Type_str, "strsub (str s, str repl, str in = NULL, bool glob = true)", f_strsub },
	{ 0, &Type_str, "paste (str delim, str a, str b)", f_strpaste },
	{ 0, &Type_str, "langcpy (str def, str lang = NULL)", f_langcpy },
	{ 0, &Type_int, "strlen (str s)", f_strlen },
	{ 0, &Type_str, "uppercase (str)", f_upper },
	{ 0, &Type_str, "lowercase (str)", f_lower },
	{ 0, &Type_str, "lexsortkey (str)", f_lexsortkey },
};


void CmdSetup_str(void)
{
	AddFuncDef(func_str, tabsize(func_str));
}
