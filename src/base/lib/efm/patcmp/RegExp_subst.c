/*
Stringsubstitution mit regulären Ausdrücken

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

#include <EFEU/memalloc.h>
#include <EFEU/mstring.h>
#include <EFEU/strbuf.h>
#include <EFEU/RegExp.h>

#define	MAXSUB	10	/* Maximale Zahl der Teilstrings */


static void put_match (StrBuf *sb, const char *str, regmatch_t *match)
{
	int i;

	if	(match->rm_so < 0)	return;

	for (i = match->rm_so; i < match->rm_eo; i++)
		sb_putc(str[i], sb);
}

static void do_repl (StrBuf *sb, const char *repl,
	const char *str, regmatch_t *match)
{
	if	(!repl)	return;

	for (; *repl != 0; repl++)
	{
		if	(*repl == '$')
		{
			repl++;

			switch (*repl)
			{
			case '0':	put_match(sb, str, match); break;
			case '1':	put_match(sb, str, match + 1); break;
			case '2':	put_match(sb, str, match + 2); break;
			case '3':	put_match(sb, str, match + 3); break;
			case '4':	put_match(sb, str, match + 4); break;
			case '5':	put_match(sb, str, match + 5); break;
			case '6':	put_match(sb, str, match + 6); break;
			case '7':	put_match(sb, str, match + 7); break;
			case '8':	put_match(sb, str, match + 8); break;
			case '9':	put_match(sb, str, match + 9); break;
			case '$':	sb_putc(*repl, sb); break;
			default:	sb_putc('$', sb); repl--; break;
			}
		}
		else	sb_putc(*repl, sb);
	}
}

/*
Die Funktion |$1| ersetzt Teilstrings von <str>, die dem regulären Ausdruck
<exp> entsprechen durch iden String <repl>. Falls <flag> verschieden von 0 ist,
werden alle gefundenen Teilstrings ersetzt, ansonsten nur der erste.

Falls <str> ein Nullpointer ist, liefert die Funktion einen Nullpointer.
Falls ein Nullpointer als regulärer Ausdruck übergeben wurde, liefert
die Funktion eine Kopie des Strings <str>.
Falls ein leerer Ausdruck für <expr> übergeben wurde, wird <repl>
zwischen jedem Zeichen von <str> eingefügt.

Der Ersatzstring <repl> kann Sequenzen der Form |\0|, |\1|, ..., |\9|
enthalten, die jeweils durch das entsprechende Teilmuster von <exp> definiert
werden.
*/

char *RegExp_subst (RegExp *exp, const char *repl, const char *str, int glob)
{
	regmatch_t match[MAXSUB];
	StrBuf *sb;
	size_t i, n;
	int flag;

	if	(str == NULL)	return NULL;
	if	(exp == NULL)	return mstrcpy(str);

	sb = sb_acquire();
	flag = 0;

	while (*str && regexec(&exp->exp, str, MAXSUB, match, flag) == 0)
	{
		if	(match[0].rm_so < 0)
		{
			match[0].rm_so = 0;
			match[0].rm_eo = strlen(str);
			n++;
		}

		for (i = 0; i < match[0].rm_so; i++)
			sb_putc(str[i], sb);

		if	(match[0].rm_eo == 0)
		{
			if	(flag)
				do_repl(sb, repl, str, match);

			sb_putc(*str, sb);
			str++;
		}
		else	do_repl(sb, repl, str, match);

		str += match[0].rm_eo;

		if	(!glob)	break;

		flag = REG_NOTBOL;
	}

	sb_puts(str, sb);
	return sb_cpyrelease(sb);
}

/*
$SeeAlso
\mref{RegExp(3)}, \mref{RegExp(7)}.
*/
