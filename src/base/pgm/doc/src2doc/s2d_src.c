/*
Dokumentation aus Sourcefile generieren

$Copyright (C) 2000 Erich Frühstück
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

#include "src2doc.h"
#include <EFEU/strbuf.h>
#include <EFEU/efio.h>

/*	Basisname
*/

static char *bname (const char *name)
{
	char *p = strrchr(name, '.');
	return p ? mstrncpy(name, p - name) : mstrcpy(name);
}

/*	Präprozessorbefehle
*/

static void pp_include (SrcData_t *data, const char *name)
{
	SrcData_copy(data, NULL);

	if	(!data->var[VAR_HEAD])
	{
		sb_puts("$header ", data->buf);
		ppcopy(data->ein, data->buf, NULL);
		SrcData_copy(data, NULL);
	}
	else	ppcopy(data->ein, NULL, NULL);
}

static void pp_define (SrcData_t *data, const char *name)
{
	char *p;

	p = parse_name(data->ein, skip_blank(data->ein));

	if	(sb_getpos(data->buf) && *p != '_')
	{
		reg_cpy(1, p);
		sb_puts("\\index[", data->synopsis);
		sb_puts(p, data->synopsis);
		sb_puts("]\n", data->synopsis);
		sb_puts(p, data->synopsis);

		if	(io_peek(data->ein) == '(')
		{
			io_t *aus = io_strbuf(data->synopsis);
			sb_putc(io_getc(data->ein), data->synopsis);
			copy_block(data->ein, aus, ')');
			io_close(aus);
		}

		sb_puts("\n\n", data->synopsis);
		SrcData_copy(data, data->tab[BUF_DESC]);
	}
	else	SrcData_copy(data, NULL);

	ppcopy(data->ein, NULL, NULL);
}

/*	Headerfiles
*/

static SrcCmd_t hdr_ppdef[] = {
	{ "define", pp_define },
};

void s2d_hdr (const char *name, io_t *ein, io_t *aus)
{
	SrcData_t data;

	SrcData_init(&data, ein, aus);
	data.var[VAR_NAME] = bname(name);
	data.var[VAR_HEAD] = msprintf(IncFmt ? IncFmt : "%#s", name);
	data.ppdef = hdr_ppdef;
	data.ppdim = tabsize(hdr_ppdef);
	data.mask = DECL_TYPE|DECL_STRUCT;
	io_printf(aus, "\\mpage[%s] %s\n", Secnum ? Secnum : "7",
		data.var[VAR_NAME]);
	reg_cpy(1, name);
	SrcData_eval(&data);
	SrcData_clean(&data);
}


/*	Sourcefiles
*/

static SrcCmd_t src_ppdef[] = {
	{ "include", pp_include },
};

void s2d_src (const char *name, io_t *ein, io_t *aus)
{
	SrcData_t data;

	SrcData_init(&data, ein, aus);
	data.var[VAR_NAME] = bname(name);
	data.ppdef = src_ppdef;
	data.ppdim = tabsize(src_ppdef);
	data.mask = DECL_VAR|DECL_FUNC;
	io_printf(aus, "\\mpage[%s] %s\n", Secnum ? Secnum : "3",
		data.var[VAR_NAME]);
	reg_cpy(1, name);
	SrcData_eval(&data);
	SrcData_clean(&data);
}
