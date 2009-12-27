/*
Formatierungshifsprogramme

$Copyright (C) 1994 Erich Frühstück
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

void ftool_addsig (StrBuf *sb, int sig, int flags);
int ftool_ioalign (IO *io, StrBuf *sb, const FmtKey *key);

#define	LOCALE(name)	(Locale.print ? Locale.print->name : NULL)
#define	NEG_SIG		LOCALE(negative_sign)
#define	POS_SIG		LOCALE(positive_sign)
#define	ZERO_SIG	LOCALE(zero_sign)


/*	Vorzeichen ausgeben
*/

void ftool_addsig (StrBuf *sb, int sig, int flags)
{
	char *p;

	if	(sig < 0)		p = NEG_SIG;
	else if	(flags & FMT_SIGN)	p = (sig > 0) ? POS_SIG : ZERO_SIG;
	else if	(flags & FMT_BLANK)	p = " ";
	else				return;

	sb_rputs(p, sb);
}


/*	Ausgeben mit Feldausrichtung
*/

int ftool_ioalign (IO *io, StrBuf *sb, const FmtKey *key)
{
	int n;

	n = 0;

	if	(key->flags & FMT_RIGHT)
		n += io_nputc(' ', io, key->width - sb->pos);

	for (; sb->pos > 0; n++)
		io_putc(sb->data[--sb->pos], io);

	n += io_nputc(' ', io, key->width - n);
	sb_destroy(sb);
	return n;
}
