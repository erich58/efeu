/*	Interpreter initialisieren
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/parsedef.h>
#include <EFEU/pconfig.h>

static int *pgm_argc = NULL;
static char **pgm_argv = NULL;

static Obj_t *pf_argc (io_t *io, void *data);
static Obj_t *pf_argv (io_t *io, void *data);
static Obj_t *pf_loadarg (io_t *io, void *data);

static ParseDef_t esh_pdef[] = {
	{ "argc", pf_argc, NULL },
	{ "argv", pf_argv, NULL },
	{ "loadarg", pf_loadarg, NULL },
};


static void f_shift (Func_t *func, void *rval, void **arg);

int EshConfig (int *narg, char **arg)
{
	int i;

	SetFunc(0, &Type_void, "shift (int n = 1)", f_shift);
	AddParseDef(esh_pdef, tabsize(esh_pdef));

	pgm_argc = narg;
	pgm_argv = arg;

	if	(*pgm_argc <= 1 || pgm_argv[1][0] == '-')
		return 0;

	(*pgm_argc)--;

	for (i = 0; i < *pgm_argc; i++)
		pgm_argv[i] = pgm_argv[i + 1];

	memfree(ProgIdent);
	ProgIdent = mstrpaste(" ", ProgName, pgm_argv[0]);

	memfree(ProgName);

	if	(pgm_argv[0] && pgm_argv[0][0] == '/')
	{
		ProgName = mstrcpy(strrchr(pgm_argv[0], '/') + 1);
	}
	else	ProgName = mstrcpy(pgm_argv[0]);

	pconfig_init();
	return 1;
}


/*	Makrodefinitionen
*/

static Obj_t *pf_loadarg (io_t *io, void *data)
{
	io_eat(io, "%s");
	loadarg(pgm_argc, pgm_argv);
	return NULL;
}

static Obj_t *pf_argc (io_t *io, void *data)
{
	return int2Obj(*pgm_argc);
}


static Obj_t *pf_argv (io_t *io, void *data)
{
	Buf_vec.type = &Type_str;
	Buf_vec.data = pgm_argv;
	Buf_vec.dim = *pgm_argc;
	return NewObj(&Type_vec, &Buf_vec);
}


/*	Funktionen
*/

static void f_shift (Func_t *func, void *rval, void **arg)
{
	int i, n;

	n = Val_int(arg[0]);

	if	(n >= *pgm_argc)
		n = *pgm_argc - 1;

	if	(n > 0)
	{
		*pgm_argc -= n;

		for (i = 1; i < *pgm_argc; i++)
			pgm_argv[i] = pgm_argv[i + n];
	}
}
