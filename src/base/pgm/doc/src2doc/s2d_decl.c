/*	Deklarationsstruktur
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include "src2doc.h"
#include <EFEU/parsub.h>

int Decl_test (Decl_t *decl, const char *name)
{
	int i;

	for (i = decl->start; i < decl->end; i++, name++) 
		if (*name != decl->def[i]) return 0;

	return (*name == 0);
}	

void Decl_print (Decl_t *decl, io_t *io)
{
	io_psub(io, "\\index[$1]\n");

	switch (decl->type)
	{
	case DECL_FUNC:
		io_puts("\\hang\n", io);
		copy_protect(decl->def, io);
		io_putc(' ', io);
		copy_protect(decl->arg, io);
		io_puts(";\n\\end\n\n", io);
		break;
	case DECL_TYPE:	
	case DECL_VAR:
		io_puts("\\hang\n", io);
		copy_protect(decl->def, io);
		io_puts(";\n\\end\n\n", io);
		break;
	case DECL_STRUCT:	
		io_printf(io, "---- verbatim\n\n%s;\n----\n\n", decl->def);
		break;
	case DECL_LABEL:
	case DECL_CALL:
	default:
		copy_protect(decl->def, io);
		io_putc(' ', io);
		copy_protect(decl->arg, io);
		io_puts("\n\n", io);
		break;
	}
}
