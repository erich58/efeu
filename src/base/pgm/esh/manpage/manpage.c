/*	Handbucheinträge
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 3.0
*/

#include <EFEU/pconfig.h>
#include <EFEU/preproc.h>
#include <EFEU/manpage.h>

static char *Input = NULL;	/* Eingabefile */
static char *Output = NULL;	/* Ausgabefile */
static char *Filter = NULL;	/* Filterprogramm */
static char *Init = NULL;	/* Initialisierungssequenzen */
static int Flag = 0;		/* Formatierungsflag */
static char *Type = NULL;	/* Formatierungstype */

Var_t vardef[] = {
	{ "Input",	&Type_str, &Input },
	{ "Output",	&Type_str, &Output },
	{ "Filter",	&Type_str, &Filter },
	{ "Type",	&Type_str, &Type },
	{ "Init",	&Type_str, &Init },
	{ "Flag",	&Type_bool, &Flag },
};

int main(int narg, char **arg)
{
	mpinput_t *input;
	mpfilter_t *mpf;

	libinit(arg[0]);
	SetupReadline();
	pconfig(NULL, vardef, tabsize(vardef));
	loadarg(&narg, arg);
	mp_setup();

	if	(Flag)
	{
		mp_show(ioerr);
		return 0;
	}

	input = NULL;
	mp_addinput(&input, Input ? io_lnum(io_fileopen(Input, "r")) : iostd);

	Output = mstrpaste(" > ", Filter, Output);
	mpf = mp_filter(io_fileopen(Output, "w"), Type);

	if	(Init)
	{
		mp_addinput(&input, io_cstr(Init));
		mp_copy(input, mpf, MPCPY_EOF);
		mp_delinput(&input);
	}

	io_eat(input->io, "%s");
	mp_copy(input, mpf, MPCPY_EOF);

	while (input != NULL)
		mp_delinput(&input);

	mp_close(mpf);
	return 0;
}
