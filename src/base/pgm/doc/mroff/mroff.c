/*	Ausgabefilter für mroff
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <mroff.h>
#include <efeudoc.h>
#include <ctype.h>

#define	NAME	"mroff"	/* Name der Variablentabelle */
#define	HDRCFG	"mroff"	/* Kopfkonfiguration */
#define	SYMTAB	"mroff"	/* Sonderzeichentabelle */


static void mr_hdr (mroff_t *mr, int mode)
{
	mroff_newline(mr);

	if	(mode)
	{
		DocDrv_eval(mr, HDRCFG);
		mr->nlignore = 1;
	}
}


io_t *DocOut_mroff (io_t *io)
{
	mroff_t *mr = DocDrv_alloc(NAME, sizeof(mroff_t));
	mr->out = io;
	mr->last = '\n';
	mr->symtab = DocSym(SYMTAB);
	mr->put = (DocDrvPut_t) mroff_putc;
	mr->hdr = (DocDrvHdr_t) mr_hdr;
	mr->rem = (DocDrvRem_t) mroff_rem;
	mr->cmd = (DocDrvCmd_t) mroff_cmd;
	mr->env = (DocDrvEnv_t) mroff_env;
	return DocDrv_io(mr);
}
