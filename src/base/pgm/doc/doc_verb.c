/*
Wörtwörtliche Ausgabe

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/mstring.h>
#include <efeudoc.h>


void Doc_verb (Doc *doc, IO *in, int base, int alt, int wrap)
{
	int32_t c;
	int pos;
	int n;
	unsigned lim1;
	unsigned lim2;

	Doc_start(doc);

	if	(base)
		io_ctrl(doc->out, DOC_BEG, base, alt);

	pos = 0;

	if	(wrap)
	{
		lim1 = 1 + 2 * (wrap / 3);
		lim2 = wrap - 1;
	}
	else	lim1 = lim2 = ~0;

	while ((c = io_getucs(in)) != EOF)
	{
		switch (c)
		{
		case 0:
		case 127:
		case '\b':
		case '\r':
		case '\f':
		case '\v':
			continue;
		case '\n':
			io_putucs(c, doc->out);
			pos = 0;
			break;
		case ' ':
			io_putucs(' ', doc->out);
			pos++;

			if	(pos >= lim1)
			{
				io_putucs('\\', doc->out);
				io_putucs('\n', doc->out);
				pos = 0;
			}

			break;
		case '\t':
			n = 8 - pos % 8;
			pos += n;

			while (n--)
				io_putucs(' ', doc->out);

			if	(pos >= lim1)
			{
				io_putucs('\\', doc->out);
				io_putucs('\n', doc->out);
				pos = 0;
			}

			break;
		default:
			if	(pos >= lim2)
			{
				io_putucs('\\', doc->out);
				io_putucs('\n', doc->out);
				pos = 0;
			}

			io_putucs(c, doc->out);
			pos++;
			break;
		}
	}

	if	(base)
		io_ctrl(doc->out, DOC_END, base, alt);
}
