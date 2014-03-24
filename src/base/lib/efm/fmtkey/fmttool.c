/*
Formatierungshifsfunktionen

$Copyright (C) 1994, 2008 Erich Frühstück
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

#include <EFEU/fmtkey.h>
#include <EFEU/locale.h>

#define	LOCALE(name)	(Locale.print ? Locale.print->name : NULL)
#define	NEG_SIG		LOCALE(negative_sign)
#define	POS_SIG		LOCALE(positive_sign)
#define	ZERO_SIG	LOCALE(zero_sign)

/*
$alias
Die Funktion |$1| schreibt das Vorzeichen einer Zahl entsprechend der
Formatangaben in umgekehrter Reihenfolge in den Stringbuffer.
*/

int ftool_addsig (StrBuf *sb, int sig, int flags)
{
	char *p;

	if	(sig < 0)		p = NEG_SIG;
	else if	(flags & FMT_SIGN)	p = (sig > 0) ? POS_SIG : ZERO_SIG;
	else if	(flags & FMT_BLANK)	p = " ";
	else				return 0;

	return sb_rputs(p, sb);
}

/*
$alias
Die Funktion |$1| schließt eine Zahlenformatierung ab. Dabei werden die ab
<pos> geschriebenen Zeichen im Stringbuffer umgedreht und entsprechend der
Formatierungsvorgaben ausgerichtet.
*/

int ftool_complete (StrBuf *sb, const FmtKey *key, int pos, int len)
{
	int n = sb->pos - pos;

	if	(key->flags & FMT_RIGHT)
		n += sb_nputc(' ', sb, key->width - n);

	sb_swap(sb, pos, n);
	n += sb_nputc(' ', sb, key->width - n);
	return n;
}

/*
$alias
Die Funktion |$1| richtet die ab <pos> in den Stringbuffer geschriebenen
Zeichen entsprechend der Formatierungsvorgaben aus.
*/

int ftool_align (StrBuf *sb, const FmtKey *key, int pos, int len)
{
	if	(key->width <= len)
		return len;

	if	(key->flags & FMT_RIGHT)
	{
		int n = sb->pos - pos;

		sb->pos -= n;
		sb_insert(sb, ' ', key->width - len);
		sb->pos += n;
	}
	else	sb_nputc(' ', sb, key->width - len);

	return key->width;
}
