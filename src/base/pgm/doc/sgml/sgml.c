/*	Ausgabefilter für SGML-Unixdoc
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <SGML.h>
#include <efeudoc.h>
#include <ctype.h>

#define	NAME	"sgml"	/* Name der Variablentabelle */
#define	HDRCFG	"sgml"	/* Kopfkonfiguration */
#define	SYMTAB	"sgml"	/* Sonderzeichentabelle */


static void sgml_hdr (SGML_t *sgml, int mode)
{
	SGML_newline(sgml, 0);

	if	(mode)
	{
		DocDrv_eval(sgml, HDRCFG);
	}
	else	io_puts(sgml->class ? "</report>\n" : "</article>\n", sgml->out);
}


io_t *DocOut_sgml (io_t *io)
{
	SGML_t *sgml = DocDrv_alloc(NAME, sizeof(SGML_t));
	sgml->out = io;
	sgml->class = 0;
	sgml->last = '\n';
	sgml->symtab = DocSym(SYMTAB);
	sgml->sym = (DocDrvSym_t) SGML_sym;
	sgml->put = (DocDrvPut_t) SGML_putc;
	sgml->hdr = (DocDrvHdr_t) sgml_hdr;
	sgml->rem = (DocDrvRem_t) SGML_rem;
	sgml->cmd = (DocDrvCmd_t) SGML_cmd;
	sgml->env = (DocDrvEnv_t) SGML_env;
	DocDrv_var(sgml, &Type_int, "class", &sgml->class);
	return DocDrv_io(sgml);
}
