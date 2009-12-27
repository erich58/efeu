/*	Dokumentation aus Sourcefile generieren
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 1.0
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

	if	(io_peek(data->ein) == '(')
	{
		io_t *aus = io_strbuf(data->synopsis);
		reg_cpy(1, p);
		sb_puts(p, data->synopsis);
		sb_putc(io_getc(data->ein), data->synopsis);
		copy_block(data->ein, aus, ')');
		io_close(aus);
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
	data.var[VAR_HEAD] = msprintf("%#s", name);
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
