/*
:*:Simple term calculating
:de:Einfache Termauswertung

$Copyright (C) 2022 Erich Frühstück
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

#include <EFEU/calculator.h>
#include <EFEU/ioscan.h>
#include <EFEU/mstring.h>

typedef struct CalcInternal CalcInternal;

#define	PRIOR_EQUAL	1
#define	PRIOR_CMP	2
#define	PRIOR_ADD	3
#define	PRIOR_MUL	4
#define	PRIOR_OBJ	5

#define	IS_CMP(p, c) (p < PRIOR_CMP && (c == '<' || c == '>'))
#define	IS_ADD(p, c) (p < PRIOR_ADD && (c == '+' || c == '-'))
#define	IS_MUL(p, c) (p < PRIOR_MUL && (c == '/' || c == '*' || c == '%'))

struct CalcInternal {
	const char *whitespace;
	const char *(*value)(IO *in, StrBuf *sb);
	IO *in;
};

#define	calc_peek(calc)	io_eat(calc->in, calc->whitespace)

static const char *parse_obj(CalcInternal *calc, StrBuf *sb)
{
	const char *p;

	if	((p = io_scan_name(calc->in, sb)))
		return calc->value(calc->in, sb);

	if	((p = io_scan_int(calc->in, sb)))
	{
		long long int val = strtoll(p, NULL, 0);
		sb_clear(sb);
		sb_printf(sb, "%lld", val);
		return sb_cstr(sb);
	}

	return io_scan_str(calc->in, sb);
}

static const char *builtin_value(IO *in, StrBuf *sb)
{
	sb_clear(sb);
	sb_putc('0', sb);
	return sb_cstr(sb);
}

static const char *parse_term(CalcInternal *, StrBuf *, int, int);

static long long int parse_value(CalcInternal *calc, StrBuf *sb,
	int prior, int delim)
{
	const char *p = parse_term(calc, sb, prior, delim);
	return p ? strtoll(p, NULL, 0) : 0;
}

static const char *parse_term(CalcInternal *calc, StrBuf *sb,
	int prior, int delim)
{
	int c = calc_peek(calc);
	sb_clear(sb);

	if	(c == EOF)
		return NULL;

	if	(c == delim)
	{
		io_getc(calc->in);
		return NULL;
	}
	
	const char *p = NULL;

	/* Prüfe auf linken Operator */

	switch (c)
	{
	case '(':
		io_getc(calc->in);
		p = parse_term(calc, sb, 0, ')');
		break;
	case '+':
	case '-':
	case '!':
		c = io_getc(calc->in);
		calc_peek(calc);
		long long int val = parse_value(calc, sb, PRIOR_OBJ, delim);

		if	(c == '-')	val = -val;
		else if	(c == '!')	val = !val;
		
		sb_clear(sb);
		sb_printf(sb, "%lld", val);
		p = sb_cstr(sb);
		break;
	default:
		p = parse_obj(calc, sb);
		break;
	}

	calc_peek(calc);

	if	(!p || prior == PRIOR_OBJ)
		return p;

	/* Hänge nachfolgende Strings ans Datenobjekt */

	do	calc_peek(calc);
	while 	(io_scan_str(calc->in, sb));

	p = sb_cstr(sb);

	/* Prüfe auf binäre Operatoren mit höherer Priorität */

	while ((c = calc_peek(calc)) != EOF)
	{
		if	(c == delim)
		{
			io_getc(calc->in);
			break;
		}

		int op = 0;

		/*	Gleichheitsoperatoren arbeiten mit Strings und werden
			deshalb gesondert behandelt
		*/

		if	(prior < PRIOR_EQUAL && (c == '!' || c == '='))
		{
			op = io_getc(calc->in);

			if	(io_peek(calc->in) != '=')
			{
				io_ungetc(op, calc->in);
				break;
			}

			io_getc(calc->in);
			StrBuf *right = sb_create(32);
			int val = mstrcmp(p, parse_term(calc, right,
				PRIOR_EQUAL, delim)) == 0;
			sb_destroy(right);
			sb_clear(sb);
			sb_printf(sb, "%d", op == '!' ? !val : val); 
			p = sb_cstr(sb);
			continue;
		}

		/* Alle anderen Operatoren */

		if	(IS_CMP(prior, c))
		{
			op = io_getc(calc->in);

			if	(io_peek(calc->in) == '=')
			{
				io_getc(calc->in);
				op = (op == '<') ? 'l' : 'g';
			}
		}
		else if	(IS_ADD(prior, c) || IS_MUL(prior, c))
		{
			op = io_getc(calc->in);
		}
		else
		{
			break;
		}

		/* Operator anwenden */

		long long int val = strtoll(p, NULL, 0);
		long long int right = 0;

		switch (op)
		{
		case '<':
			val = val < parse_value(calc, sb, PRIOR_CMP, delim);
			break;
		case 'l':
			val = val <= parse_value(calc, sb, PRIOR_CMP, delim);
			break;
		case 'g':
			val = val >= parse_value(calc, sb, PRIOR_CMP, delim);
			break;
		case '>':
			val = val > parse_value(calc, sb, PRIOR_CMP, delim);
			break;
		case '+':
			val += parse_value(calc, sb, PRIOR_ADD, delim);
			break;
		case '-':
			val -= parse_value(calc, sb, PRIOR_ADD, delim);
			break;
		case '*':
			val *= parse_value(calc, sb, PRIOR_MUL, delim);
			break;
		case '/':
			right = parse_value(calc, sb, PRIOR_MUL, delim);
			val = right ? val / right : 0;
			break;
		case '%':
			right = parse_value(calc, sb, PRIOR_MUL, delim);
			val = right ? val % right : 0;
			break;
		}

		sb_clear(sb);
		sb_printf(sb, "%lld", val);
		p = sb_cstr(sb);
	}

	return p;
}

const char *calc_eval(Calculator *calc, IO *in, StrBuf *sb)
{
	CalcInternal ci;

	if	(!in)
		return NULL;

	ci.value = (calc && calc->value) ? calc->value : builtin_value;
	ci.in = in;
	ci.whitespace = " \t";

	if	(!sb)
	{
		static STRBUF(buf, 32);
		sb = &buf;
		sb_clear(sb);
	}

	const char *p = parse_term(&ci, sb, 0, EOF);
	return p;
}
