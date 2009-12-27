/*
Zeichenketten, Version 2

$Copyright (C) 2007 Erich Frühstück
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
#include <EFEU/printobj.h>
#include <EFEU/cmdconfig.h>
#include <EFEU/StrData.h>
#include <ctype.h>

size_t Read_varstr (const EfiType *st, void *data, IO *io)
{
	int byte = io_getc(io);

	if	(byte == EOF)
	{
		StrData_set(data, NULL);
		return 0;
	}
	else if	(byte > 0)
	{
		size_t size = io_getval(io, byte);
		StrData_mset(data, io_mread(io, size));
		return size + byte + 1;
	}
	else
	{
		StrData_set(data, NULL);
		return 1;
	}
}

size_t Write_varstr (const EfiType *st, const void *data, IO *io)
{
	return io_putstr(StrData_get(data), io);
}

int Print_varstr (const EfiType *st, const void *data, IO *io)
{
	char *str = StrData_get(data);

	if	(str)
	{
		int n = io_puts("\"", io);
		n += io_xputs(str, io, "\"");
		n += io_puts("\"", io);
		return n;
	}
	else	return io_puts("NULL", io);
}

void Clean_varstr (const EfiType *st, void *data, int mode)
{
	StrData_clean(data, mode);
}

void Copy_varstr (const EfiType *st, void *tg, const void *src)
{
	StrData_copy(tg, src);
}

EfiType Type_varstr = COMPLEX_TYPE("varstr", "StrData", sizeof(StrData), 0,
	Read_varstr, Write_varstr, Print_varstr,
	NULL, NULL, Clean_varstr, Copy_varstr, TYPE_MALLOC);

static EfiObj *varstr_type (const EfiObj *obj, void *data)
{
	StrData *sd = obj->data;
	return str2Obj(mstrcpy(sd->ctrl ? sd->ctrl->name : "null"));
}

static EfiMember member_varstr[] = {
	{ "type", &Type_str, varstr_type, NULL },
};

static void str_to_varstr (EfiFunc *func, void *rval, void **arg)
{
	StrData_set(rval, Val_str(arg[0]));
}

static void ptr_to_varstr (EfiFunc *func, void *rval, void **arg)
{
	StrData_set(rval, NULL);
}

static void varstr_to_str (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(StrData_get(arg[0]));
}

#if	0
#define	RVSTR	Val_str(rval)
#define	CHAR(n)	Val_char(arg[n])
#define	STR(n)	StrData_get(arg[n])
#define	INT(n)	Val_int(arg[n])

#define	EXPR(name, expr)	\
CEXPR(name, StrData_get(rval) = expr)

#define	COMPARE(name, op)	\
CEXPR(name, Val_int(rval) = mstrcmp(STR(0), STR(1)) op 0)


/*	Konvertierungsfunktionen
*/

CEXPR(c_int32, *((int32_t *) rval) = *((int32_t *) arg[0]))
CEXPR(k_wchar2int, *((int *) rval) = *((int32_t *) arg[0]))
CEXPR(k_int2wchar, *((int32_t *) rval) = *((int *) arg[0]))

CEXPR(k_str2int, Val_int(rval) = STR(0) ?
	(int) strtol(STR(0), NULL, 0) : 0)
CEXPR(k_str2uint, Val_uint(rval) = STR(0) ?
	(unsigned) strtoul(STR(0), NULL, 0) : 0)
CEXPR(c_str2int, Val_int(rval) = STR(0) ?
	(int) strtol(STR(0), NULL, Val_int(arg[1])) : 0)
CEXPR(c_str2uint, Val_uint(rval) = STR(0) ?
	(unsigned) strtoul(STR(0), NULL, Val_int(arg[1])) : 0)

static void k_str2double (EfiFunc *func, void *rval, void **arg)
{
	char *def = StrData_get(arg[0]);
 	Val_double(rval) = def ? C_strtod(def, NULL) : 0.;
}

static void f_dstrcut (EfiFunc *func, void *rval, void **arg)
{
	char **s = arg[0];

	if	(*s)
	{
		char *p = NULL;
		Val_double(rval) = strtod(*s, &p);

		if	(p != *s)
		{
			p = mstrcpy(p);
			memfree(*s);
			*s = p;
		}
	}
	else	Val_double(rval) = 0.;
}

EXPR(k_ptr2str, NULL)
EXPR(k_char2str, msprintf("%c", Val_char(arg[0])))
EXPR(k_double2str, DoubleToString(Val_double(arg[0])))

static void to_int32 (EfiFunc *func, void *rval, void **arg)
{
	*((int32_t *) rval) = mstr2int64(StrData_get(arg[0]), NULL,
		func->dim > 1 ? Val_int(arg[1]) : 0);
}

static void to_int64 (EfiFunc *func, void *rval, void **arg)
{
	*((int64_t *) rval) = mstr2int64(StrData_get(arg[0]), NULL,
		func->dim > 1 ? Val_int(arg[1]) : 0);
}

static void to_uint32 (EfiFunc *func, void *rval, void **arg)
{
	*((uint32_t *) rval) = mstr2uint64(StrData_get(arg[0]), NULL,
		func->dim > 1 ? Val_int(arg[1]) : 0);
}

static void to_uint64 (EfiFunc *func, void *rval, void **arg)
{
	*((uint64_t *) rval) = mstr2uint64(StrData_get(arg[0]), NULL,
		func->dim > 1 ? Val_int(arg[1]) : 0);
}

COMPARE(b_str_lt, <)
COMPARE(b_str_le, <=)
COMPARE(b_str_eq, ==)
COMPARE(b_str_ne, !=)
COMPARE(b_str_ge, >=)
COMPARE(b_str_gt, >)
#endif

static void strmul (StrData *tg, const char *str, int n)
{
	if	(str && n > 0)
	{
		StrBuf *sb;

		StrData_strbuf(tg);
		sb = tg->ptr;

		for (; n > 0; n--)
			sb_puts(str, sb);

		sb_putc(0, sb);
	}
	else	StrData_clean(tg, 1);
}

static void f_str_lmul (EfiFunc *func, void *rval, void **arg)
{
	strmul(rval, StrData_get(arg[1]), Val_int(arg[0]));
}

static void f_str_rmul (EfiFunc *func, void *rval, void **arg)
{
	strmul(rval, StrData_get(arg[0]), Val_int(arg[1]));
}

#if	0

EXPR(f_str_add,	mstrpaste(NULL, STR(0), STR(1)))
EXPR(f_strpaste, mstrpaste(STR(0), STR(1), STR(2)))
EXPR(f_langcpy, mlangcpy(STR(0), STR(1)))
CEXPR(f_strlen, Val_int(rval) = STR(0) ? strlen(STR(0)) : 0)

/*	Spezialfunktionen
*/

static void k_nchar2str (EfiFunc *func, void *rval, void **arg)
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
	StrData_get(rval) = s;
}

static void k_type2str (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb;
	IO *io;

	sb = sb_create(0);
	io = io_strbuf(sb);
	PrintType(io, Val_type(arg[0]), Val_int(arg[1]));
	io_close(io);
	StrData_get(rval) = sb2str(sb);
}

static void f_str_index (EfiFunc *func, void *rval, void **arg)
{
	register char *s = STR(0);
	register int i = Val_int(arg[1]);
	Val_char(rval) = (s && i < strlen(s)) ? s[i] : 0;
}

static void f_substr (EfiFunc *func, void *rval, void **arg)
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

static void f_strcut (EfiFunc *func, void *rval, void **arg)
{
	char *s;
	char *p;
	int i;
	int mode;

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
		mode = INT(2);

		if	(mode > 0)
		{
			while (s[i] && listcmp(p, s[i]))
				i++;
		}
		else if	(mode < 0)
		{
			--i;
		}

		STR(0) = s[i] ? mstrcpy(s + i) : NULL;
		memfree(s);
		return;
	}

	RVSTR = s;
	STR(0) = NULL;
}

static void f_xstrcut (EfiFunc *func, void *rval, void **arg)
{
	StrBuf *sb;
	char *delim, *base;
	IO *in, *out;
	int c;
	int mode;

	if	((base = STR(0)) == NULL)
	{
		RVSTR = NULL;
		return;
	}

	in = io_cstr(base);
	delim = STR(1);
	sb = sb_create(0);
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
	mode = INT(2);

	if	(mode < 0)
		io_ungetc(c, in);

	if	(mode > 0)
		io_eat(in, delim);

	while ((c = io_getc(in)) != EOF)
		sb_putc(c, sb);

	STR(0) = sb2str(sb);
	io_close(in);
	memfree(base);
}

static void f_strsub (EfiFunc *func, void *rval, void **arg)
{
	char *s, *p;
	char *repl;
	char *in;
	int flag;
	StrBuf *sb;

	if	((s = STR(0)) == NULL)
	{
		RVSTR = NULL;
		return;
	}

	repl = STR(1);
	in = STR(2);
	flag = ! Val_bool(arg[3]);
	sb = sb_create(0);

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

static void f_upper (EfiFunc *func, void *rval, void **arg)
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

static void f_lower (EfiFunc *func, void *rval, void **arg)
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

static void f_lexsortkey (EfiFunc *func, void *rval, void **arg)
{
	RVSTR = mstrcpy(lexsortkey(StrData_get(arg[0]), NULL));
}
#endif


static EfiFuncDef func_varstr[] = {
	{ FUNC_PROMOTION, &Type_str, "varstr ()", varstr_to_str },
	{ 0, &Type_varstr, "str ()", str_to_varstr },
	{ 0, &Type_varstr, "varstr (_Ptr_)", ptr_to_varstr },
#if	0
	{ FUNC_PROMOTION, &Type_int, "char ()", k_char2int },
	{ 0, &Type_char, "char (int)", k_int2char },

	{ FUNC_PROMOTION, &Type_int, "wchar_t ()", k_wchar2int },
	{ FUNC_PROMOTION, &Type_int32, "wchar_t ()", c_int32 },
	{ 0, &Type_wchar, "wchar_t (int)", k_int2wchar },
	{ 0, &Type_wchar, "wchar_t (int32_t)", c_int32 },

	{ 0, &Type_int, "int (str)", k_str2int },
	{ 0, &Type_int, "int (str, int base)", c_str2int },
	{ 0, &Type_uint, "unsigned (str)", k_str2uint },
	{ 0, &Type_uint, "unsigned (str, int base)", c_str2uint },

	{ 0, &Type_int32, "int32_t (str)", to_int32 },
	{ 0, &Type_int32, "int32_t (str, int base)", to_int32 },
	{ 0, &Type_uint32, "uint32_t (str)", to_uint32 },
	{ 0, &Type_uint32, "uint32_t (str, int base)", to_uint32 },

	{ 0, &Type_int64, "int64_t (str)", to_int64 },
	{ 0, &Type_int64, "int64_t (str, int base)", to_int64 },
	{ 0, &Type_varint, "varint (str)", to_int64 },
	{ 0, &Type_varint, "varint (str, int base)", to_int64 },
	{ 0, &Type_uint64, "uint64_t (str)", to_uint64 },
	{ 0, &Type_uint64, "uint64_t (str, int base)", to_uint64 },
	{ 0, &Type_varsize, "varsize (str)", to_uint64 },
	{ 0, &Type_varsize, "varsize (str, int base)", to_uint64 },

	{ 0, &Type_double, "double (str, bool locale = false)", k_str2double },
	{ 0, &Type_str, "char ()", k_char2str },
	{ 0, &Type_str, "str (double)", k_double2str },
	{ 0, &Type_str, "str (char, int = 1)", k_nchar2str },
	{ 0, &Type_str, "str (Type_t, int verbosity = 1)", k_type2str },
	{ 0, &Type_str, "str (_Ptr_)", k_ptr2str },

	{ FUNC_VIRTUAL, &Type_char, "operator[] (str, int)", f_str_index },
	{ FUNC_VIRTUAL, &Type_str, "operator+ (str, str)", f_str_add },
#endif
	{ FUNC_VIRTUAL, &Type_varstr, "operator* (int, varstr)", f_str_lmul },
	{ FUNC_VIRTUAL, &Type_varstr, "operator* (varstr, int)", f_str_rmul },
#if 0
	{ FUNC_VIRTUAL, &Type_bool, "operator< (str, str)", b_str_lt },
	{ FUNC_VIRTUAL, &Type_bool, "operator<= (str, str)", b_str_le },
	{ FUNC_VIRTUAL, &Type_bool, "operator== (str, str)", b_str_eq },
	{ FUNC_VIRTUAL, &Type_bool, "operator!= (str, str)", b_str_ne },
	{ FUNC_VIRTUAL, &Type_bool, "operator>= (str, str)", b_str_ge },
	{ FUNC_VIRTUAL, &Type_bool, "operator> (str, str)", b_str_gt },

	{ 0, &Type_str, "substr (str s, int pos, int len = 0)", f_substr },
	{ 0, &Type_str, "strcut (str & s, str delim, int mode = 1)",
		f_strcut },
	{ 0, &Type_str, "xstrcut (str & s, str delim, int mode = 1)",
		f_xstrcut },
	{ 0, &Type_double, "dstrcut (str & s)", f_dstrcut },
	{ 0, &Type_str, "strsub (str s, str repl, str in = NULL, "
		"bool glob = true)", f_strsub },
	{ 0, &Type_str, "paste (str delim, str a, str b)", f_strpaste },
	{ 0, &Type_str, "langcpy (str def, str lang = NULL)", f_langcpy },
	{ 0, &Type_int, "strlen (str s)", f_strlen },
	{ 0, &Type_str, "uppercase (str)", f_upper },
	{ 0, &Type_str, "lowercase (str)", f_lower },
	{ 0, &Type_str, "lexsortkey (str)", f_lexsortkey },
#endif
};

void CmdSetup_varstr(void)
{
	AddEfiMember(Type_varstr.vtab, member_varstr, tabsize(member_varstr));
	AddFuncDef(func_varstr, tabsize(func_varstr));
}
