/*	Filter für Terminalausgabe
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/object.h>
#include <EFEU/ftools.h>
#include <EFEU/parsedef.h>
#include <EFEU/pconfig.h>
#include <EFEU/preproc.h>
#include "printpar.h"

/*	Printcapvariablen
*/

#define	CAP_SUFFIX	"cap"

char *pc_device = NULL;
char *pc_init = NULL;
char *pc_exit = NULL;
char *pc_newline = "\n";
char *pc_newpage = "\f";
char *pc_ulfmt = NULL;

VarDef_t pcvar[] = {
	{ "pc_dev",	&Type_str, &pc_device },
	{ "pc_init",	&Type_str, &pc_init },
	{ "pc_exit",	&Type_str, &pc_exit },
	{ "pc_ff",	&Type_str, &pc_newpage },
	{ "pc_nl",	&Type_str, &pc_newline },
	{ "pc_ul",	&Type_str, &pc_ulfmt },
};

static Obj_t *p_fonttab (io_t *io, void *data)
{
	if	(io_eat(io, "%s") == '{')
	{
		io_getc(io);
		loadfont(io, "}");
	}

	return NULL;
}

static Obj_t *p_ulmode (io_t *io, void *data)
{
	ulmode = (int) (size_t) data;
	return NULL;
}

static Obj_t *p_fillmode (io_t *io, void *data)
{
	fillmode = (int) (size_t) data;
	return NULL;
}

static ParseDef_t pdef[] = {
	{ "fonttab", p_fonttab, NULL },
	{ "ul_on", p_ulmode, (void *) 1 },
	{ "ul_off", p_ulmode, (void *) 0 },
	{ "fill_on", p_fillmode, (void *) 1 },
	{ "fill_off", p_fillmode, (void *) 0 },
};

/*	Initialisierung
*/

io_t *pc_setup (const char *name)
{
	char *p;
	io_t *io;
	VarTab_t *vtab;

	p = fsearch(PCPath, NULL, name, CAP_SUFFIX);

	if	(p == NULL)
	{
		reg_cpy(1, name);
		liberror(NULL, 1);
	}

	vtab = VarTab("PrintCap", tabsize(pcvar));
	AddVarDef(vtab, pcvar, tabsize(pcvar));
	PushVarTab(vtab, NULL);
	AddParseDef(pdef, tabsize(pdef));

	io = io_fileopen(p, "r");
	io = io_cmdpreproc(io);
	io_eval(io, NULL);
	io_close(io);

	PopVarTab();
	/*
	DelVarTab(vtab);
	*/
	memfree(p);
	return io_fileopen(pc_device, "w");
}
