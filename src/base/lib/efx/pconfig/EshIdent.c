/*
:*:esh script identification
:de:Esh-Skript Identifikation

$Copyright (C) 1995 Erich Frühstück
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

#include <EFEU/parsedef.h>
#include <EFEU/pconfig.h>
#include <EFEU/ioctrl.h>
#include <EFEU/CmdPar.h>
#include <EFEU/Resource.h>
#include <EFEU/Debug.h>

void EshIdent (io_t *in)
{
	CmdPar_t *par;
	char *desc;
	char *p, *s;

	io_peek(in);
	io_ctrl(in, IOPP_COMMENT, &desc);

	if	(!desc)	return;

	par = CmdPar_ptr(NULL);

	for (p = desc; *p != 0; p++)
	{
		if	(*p == '\n' && p[1] == '\n')
		{
			p++;
			*p = 0;
			p++;
			CmdPar_setval(par, "Ident", mlangcpy(desc, NULL));
			break;
		}
	}

	for (; *p != 0; p++)
	{
		if	(*p == '$' && (p[1] == 'C' || p[1] == 'c'))
		{
			p++;

			if	((s = strchr(p, '\n')) != NULL)
				*s = 0;

			CmdPar_setval(par, "Copyright", mstrcpy(p));
		}
	}

	memfree(desc);
}
