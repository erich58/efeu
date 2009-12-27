/*
Daten einlesen
*/

#include <EFEU/EDB.h>
#include <EFEU/ioctrl.h>
#include <EFEU/printobj.h>
#include <EFEU/DBData.h>
#include <EFEU/parsearg.h>
#include <EFEU/EfiInput.h>
#include <ctype.h>

#define	ERR_DATA "[edb:input]$!: type $1: undefined input method $2.\n"

void edb_data (EDB *edb, IO *io, const char *def)
{
	AssignArg *mode;
	EfiInput *idef;
	EfiType *type;

	edb->save = 0;
	type = edb->obj ? edb->obj->type : NULL;

	mode = assignarg(def, NULL, NULL);
	idef = mode ? SearchEfiPar(type, &EfiPar_input, mode->name) :
		&EfiInput_plain;

	if	(idef)
	{
		void *par;
		char *opt = mode ? mode->opt : NULL;
		char *arg = mode ? mode->arg : NULL;

		if	(arg && arg[0] == '?')
		{
			PrintEfiPar(ioerr, idef);
			exit(EXIT_SUCCESS);
		}

		par = idef->in_open ? idef->in_open(io, type,
			idef->par, opt, arg) : io;
		edb_input(edb, idef->in_read, par);
	}
	else	log_error(edb_err,  ERR_DATA, "ss", type->name, mode->name);

	memfree(mode);
}
