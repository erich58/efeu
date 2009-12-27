/*	Ausgabefilter für Terminal
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <term.h>
#include <efeudoc.h>
#include <ctype.h>

#define	TPARM	"tparm"	/* Terminalkonfiguration */
#define	NAME	"term"	/* Name der Variablentabelle */
#define	HDRCFG	"term"	/* Kopfkonfiguration */
#define	SYMTAB	"term"	/* Sonderzeichentabelle */

TermPar_t TermPar;

static Var_t term_var[] = {
	{ "wpmargin", &Type_int, &TermPar.wpmargin },
	{ "rm", &Type_str, &TermPar.rm },
	{ "it", &Type_str, &TermPar.it },
	{ "bf", &Type_str, &TermPar.bf },
	{ "tt", &Type_str, &TermPar.tt },
};

void TermPar_init(void)
{
	static int need_init = 1;

	if	(need_init)
	{
		io_t *in;

		in = io_findopen(CFGPATH, TPARM, CFGEXT, "r");
		in = io_lnum(in);
		PushVarTab(NULL, NULL);
		AddVar(NULL, term_var, tabsize(term_var));
		io_eval(in, NULL);
		PopVarTab();
		io_close(in);
		need_init = 0;
	}
}

static void term_hdr (term_t *trm, int mode)
{
	if	(mode)	DocDrv_eval(trm, HDRCFG);
}

static void term_sym (term_t *trm, const char *name)
{
	if	(mstrcmp("nbsp", name) == 0)
	{
		term_hmode(trm);
		io_putc(' ', trm->out);
		trm->col++;
	}
	else if	(mstrcmp("shy", name) == 0)
	{
		if	(trm->col >= TermPar.wpmargin)
		{
			io_putc('-', trm->out);
			term_newline(trm, 0);
		}
	}
	else
	{
		char *sym = DocSym_get(trm->symtab, name);

		term_hmode(trm);

		if	(sym)	io_puts(sym, trm->out);
		else		io_printf(trm->out, "[%s]", name);
	}
}

io_t *DocOut_term (io_t *io)
{
	term_t *trm = DocDrv_alloc(NAME, sizeof(term_t));
	trm->out = io;
	trm->margin = 2 * TERM_INDENT;
	trm->symtab = DocSym(SYMTAB);
	trm->sym = (DocDrvSym_t) term_sym;
	trm->put = (DocDrvPut_t) term_putc;
	trm->hdr = (DocDrvHdr_t) term_hdr;
	trm->cmd = (DocDrvCmd_t) term_cmd;
	trm->env = (DocDrvEnv_t) term_env;
	AddVar(trm->vartab, term_var, tabsize(term_var));
	DocDrv_var(trm, &Type_int, "margin", &trm->margin);
	TermPar_init();
	return DocDrv_io(trm);
}
