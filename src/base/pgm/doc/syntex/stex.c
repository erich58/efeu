/*	Ausgabefilter für SynTeX
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <SynTeX.h>
#include <efeudoc.h>
#include <ctype.h>

#define	NAME	"SynTeX"	/* Name der Variablentabelle */
#define	HDRCFG	"SynTeX"	/* Kopfkonfiguration */
#define	SYMTAB	"latex"		/* Sonderzeichentabelle */


static void stex_hdr (SynTeX_t *stex, int mode)
{
	if	(mode)
	{
		DocDrv_eval(stex, HDRCFG);
	}
	else	io_puts("\n\\end{document}\n", stex->out);
}


io_t *DocOut_syntex (io_t *io)
{
	SynTeX_t *stex = DocDrv_alloc(NAME, sizeof(SynTeX_t));
	stex->out = io;
	stex->last = '\n';
	stex->symtab = DocSym(SYMTAB);
	stex->put = (DocDrvPut_t) SynTeX_putc;
	stex->hdr = (DocDrvHdr_t) stex_hdr;
	stex->rem = (DocDrvRem_t) SynTeX_rem;
	stex->cmd = (DocDrvCmd_t) SynTeX_cmd;
	stex->env = (DocDrvEnv_t) SynTeX_env;
	return DocDrv_io(stex);
}
