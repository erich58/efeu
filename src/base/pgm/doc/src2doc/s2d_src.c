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
#include <EFEU/CmdPar.h>

/*	Basisname
*/

static char *bname (const char *name)
{
	char *p = strrchr(name, '.');
	return p ? mstrncpy(name, p - name) : mstrcpy(name);
}

/*	Präprozessorbefehle
*/

static void pp_include (SrcData *data, const char *name)
{
	SrcData_copy(data, NULL, NULL);

	if	(!data->doc.var[VAR_HEAD])
	{
		sb_puts("$header ", data->buf);
		ppcopy(data->ein, data->buf, NULL);
		SrcData_copy(data, NULL, NULL);
	}
	else	ppcopy(data->ein, NULL, NULL);
}

static void pp_define (SrcData *data, const char *name)
{
	char *p;

	p = parse_name(data->ein, skip_blank(data->ein));

	if	(sb_getpos(data->buf) && *p != '_')
	{
		sb_puts("\\index[", data->doc.synopsis);
		sb_puts(p, data->doc.synopsis);
		sb_puts("]\n", data->doc.synopsis);
		sb_puts(p, data->doc.synopsis);

		if	(io_peek(data->ein) == '(')
		{
			IO *aus = io_strbuf(data->doc.synopsis);
			sb_putc(io_getc(data->ein), data->doc.synopsis);
			copy_block(data->ein, aus, ')', 1);
			io_close(aus);
		}

		sb_puts("\n\n", data->doc.synopsis);
		SrcData_copy(data, data->doc.tab[BUF_DESC], p);
	}
	else	SrcData_copy(data, NULL, p);

	ppcopy(data->ein, NULL, NULL);
}

/*	Headerfiles
*/

static SrcCmd hdr_ppdef[] = {
	{ "define", pp_define },
};

void s2d_hdr (const char *name, IO *ein, IO *aus)
{
	SrcData data;

	SrcData_init(&data, ein);
	data.doc.var[VAR_NAME] = bname(name);
	data.doc.var[VAR_HEAD] = msprintf(IncFmt ? IncFmt : "%#s", name);
	data.ppdef = hdr_ppdef;
	data.ppdim = tabsize(hdr_ppdef);
	data.mask = DECL_TYPE|DECL_STRUCT;
	io_printf(aus, "\\mpage[%s] %s\n", Secnum ? Secnum : "7",
		data.doc.var[VAR_NAME]);
	SrcData_eval(&data, name);
	SrcData_write(&data, aus);
}


/*	Sourcefiles
*/

static SrcCmd src_ppdef[] = {
	{ "include", pp_include },
};

void s2d_src (const char *name, IO *ein, IO *aus)
{
	SrcData data;

	SrcData_init(&data, ein);
	data.doc.var[VAR_NAME] = bname(name);
	data.ppdef = src_ppdef;
	data.ppdim = tabsize(src_ppdef);
	data.mask = DECL_VAR|DECL_FUNC;
	data.xmask = DECL_SVAR|DECL_SFUNC|DECL_TYPE|DECL_STRUCT;
	io_printf(aus, "\\mpage[%s] %s\n", Secnum ? Secnum : "3",
		data.doc.var[VAR_NAME]);
	SrcData_eval(&data, name);
	SrcData_write(&data, aus);
}

/*	Scriptfiles
*/

static SrcCmd cmd_ppdef[] = {
	{ "define", pp_define },
};

void s2d_cmd (const char *name, IO *ein, IO *aus)
{
	SrcData data;

	SrcData_init(&data, ein);
	data.doc.var[VAR_NAME] = bname(name);
	data.ppdef = cmd_ppdef;
	data.ppdim = tabsize(cmd_ppdef);
	data.mask = DECL_TYPE|DECL_STRUCT|DECL_VAR|DECL_FUNC;
	io_printf(aus, "\\mpage[%s] %s\n", Secnum ? Secnum : "7",
		data.doc.var[VAR_NAME]);
	SrcData_eval(&data, name);
	SrcData_write(&data, aus);
}
