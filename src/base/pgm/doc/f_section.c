/*	Gliederungsbefehl
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include "efeudoc.h"
#include <ctype.h>


#define	DOC	Val_ptr(arg[0])
#define	TYPE	Val_char(arg[1])
#define	THEAD	Val_str(arg[2])
#define	HEAD	Val_str(arg[3])

void DocFunc_section (Func_t *func, void *rval, void **arg)
{
	Doc_t *doc = DOC;
	int type, mode;
	char *p;
	io_t *in;
	
	if	(doc == NULL)	return;

	Doc_start(doc);

/*	Gliederungsbefehl und Umgebungsbereinigung bestimmen
*/
	type = DOC_SEC_PART;
	mode = 0;

	switch (Val_char(arg[1]))
	{
	case 'P':	mode = 0; type = DOC_SEC_PART; break;
	case 'C':	mode = 0; type = DOC_SEC_CHAP; break;
	case 'c':	mode = 0; type = DOC_SEC_MCHAP; break;
	case 'S':	mode = 1; type = DOC_SEC_SECT; break;
	case 's':	mode = 1; type = DOC_SEC_SSECT; break;
	case 'p':	mode = 1; type = DOC_SEC_PARA; break;
	case 'H':	mode = 1; type = DOC_SEC_HEAD; break;
	case 'h':	mode = 2; type = DOC_SEC_SHEAD; break;
	case 'm':	mode = 2; type = DOC_SEC_MARG; break;
	case 'F':	mode = 2; type = DOC_SEC_CAPT; break;
	case 'f':	mode = 2; type = DOC_SEC_SCAPT; break;
	case 'N':	mode = 2; type = DOC_SEC_NOTE; break;
	case 'n':	mode = 2; type = DOC_SEC_FNOTE; break;
	default:	break;
	}

/*	Umgebungsbereinigung
*/
	if	(mode == 2)
	{
		if	(DOC_IS_LIST(doc->env.type))
			Doc_endenv(doc);

		while	(Doc_islist(doc))
		{
			Doc_endenv(doc);
			Doc_endenv(doc);
		}
	}
	else	Doc_endall(doc, mode);

	Doc_par(doc);
	Doc_stdpar(doc, 0);

	if	(doc->env.pflag)
		io_putc('\n', doc->out);

/*	Gliederungsbefehl ausgeben
*/
	p = Doc_expand(doc, io_cstr(THEAD), 1);
	io_ctrl(doc->out, DOC_BEG, type, p);
	memfree(p);
	in = io_cstr(HEAD);
	doc->env.hmode = 2;
	Doc_copy(doc, in);
	doc->env.hmode = 0;
	io_close(in);
	io_ctrl(doc->out, DOC_END, type, p);
	io_putc('\n', doc->out);
	doc->env.pflag = (mode < 2);
}
