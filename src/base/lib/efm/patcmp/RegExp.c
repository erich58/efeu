/*
Reguläre Ausdrücke

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

#include <EFEU/memalloc.h>
#include <EFEU/mstring.h>
#include <EFEU/RegExp.h>

static char err_buf[256];

static char *re_ident (RegExp_t *exp)
{
	return msprintf("%s%#s", (exp->icase ? "ICASE " : NULL), exp->def);
}

static RegExp_t *re_admin (RegExp_t *tg, const RegExp_t *src)
{
	if	(tg)
	{
		regfree(&tg->exp);
		memfree(tg->def);
		memfree(tg);
		return NULL;
	}
	else	return memalloc(sizeof(RegExp_t));
}

ADMINREFTYPE(RegExp_reftype, "RegExp", re_ident, re_admin);

char *RegExp_error = NULL;

/*
Die Funktion |$1| konvertiert den String <str> in einen regulären
Ausdruck (<"POSIX Extended Regular Expression">).
Bei einem Syntaxfehler liefert die Funktion einen
Nullpointer und die Variable |RegExp_error| verweist auf einen String
mit der Fehlerbeschreibung.
Bei erfolgreicher Verarbeitung wird |RegExp_error| auf |NULL| gesetzt.

Wird ein Nullpointer als Argument übergeben, liefert die
Funktion ebenfalls einen Nullpointer. Dies gilt nicht als Fehler.

Falls <flag> verschieden von 0 ist, wird zwischen Groß- und Kleinschreibung
nicht unterschieden.
*/

RegExp_t *RegExp (const char *str, int flag)
{
	regex_t buf;
	int n;

	RegExp_error = NULL;

	if	(str == NULL)	return NULL;

	n = regcomp(&buf, str, REG_EXTENDED | (flag ? REG_ICASE : 0));

	if	(n == 0)
	{
		RegExp_t *x = rd_create(&RegExp_reftype);
		x->exp = buf;
		x->def = mstrcpy(str);
		x->icase = flag;
		return x;
	}

	regerror(n, &buf, err_buf, sizeof(err_buf));
	RegExp_error = err_buf;
	return NULL;
}

static regmatch_t match[REGEXP_MAXSUB];

/*
Die Funktion |$1| vergleicht den regulären Ausdruck <exp> mit
dem String <str>. Falls der String nicht mit dem regulären Ausdruck
übereinstimmt, liefert die Funktion 0 und falls Pointer <ptr>
nicht Null ist, wird ein Nullpointer dort gespeichert.

Bei einer Übereinstimmung und
falls der Pointer <ptr> verschieden von Null ist, wird dort die Adresse
eines Vektors mit Offsetwerten der Teilmuster gespeichert und
die Funktion liefert die Zahl der Teilmuster (größer als 0).
Es können maximal |REGEX_MAXSUB| Teilmuster abgefragt werden.
Ansonsten liefert die Funktion bei Übereinstimmung den Wert 1.

ACHTUNG: Falls sowohl <exp> als auch <str> ein Nullpointer ist,
wird <ptr> auf |NULL| gesetzt und die Funktion liefert 1 und nicht 0!
*/

int RegExp_exec (RegExp_t *exp, const char *str, regmatch_t **ptr)
{
	int n;

	if	(ptr)	*ptr = NULL;

	if	(exp == NULL)	return (str ? 0 : 1);
	if	(str == NULL)	return 0;

	n = regexec(&exp->exp, str, REGEXP_MAXSUB, match, 0);

	if	(n != 0)	return 0;
	if	(!ptr)		return 1;

	for (n = 0; n < REGEXP_MAXSUB; n++)
		if	(match[n].rm_so < 0)	break;

	if	(n == 0)
	{
		match[n].rm_so = 0;
		match[n].rm_eo = strlen(str);
		n++;
	}

	*ptr = match;
	return n;
}

/*
$SeeAlso
\mref{RegExp_sub(3)},
\mref{regcomp(3)}, \mref{regexec(3)}, \mref{regex(7)}, GNU regex manual.
*/
