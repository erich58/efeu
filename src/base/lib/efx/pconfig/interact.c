/*	Interaktive Ein/Ausgabe
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/pconfig.h>
#include <EFEU/ioctrl.h>
#include <EFEU/extension.h>

#define	PROMPT_STD	PS1
#define	PROMPT_SUB	PS2
#define	PROMPT_MARK	"> "

#if	HAS_ISATTY
extern int isatty (int field);
#endif

/*	Hilfsvariablen für Standardeingabe
*/

static int line = 0;
static int mode_depth = 0;
static int iflag = 1;
static int mark = 1;
static char *prompt = NULL;


/*	Hilfsfunktionen
*/

static int ia_get (void *io);
static int ia_put (int c, void *io);
static int ia_ctrl (void *io, int req, va_list list);


/*	Standardeingabe
*/

#if	0
static io_t ios_ia = STD_IODATA(ia_get, ia_put, ia_ctrl, NULL);
#endif

static int batchmode = 0;

static Var_t ia_var[] = {
	{ "batchmode",	&Type_int, &batchmode, 0 },
};


/*	Initialisierungsstruktur
*/

void SetupInteract(void)
{
#if	HAS_ISATTY
	if	(!(isatty(0) && isatty(1)))
		batchmode = 1;
#else
	applfile("batch", APPL_APP);
#endif
	AddVar(LocalVar, ia_var, tabsize(ia_var));
}


/*	Lesen von der Standardeingabe
*/

static int ia_get(void *ptr)
{
	int c;

	if	(iflag && !batchmode)
	{
		char *p;

		if	(mark)		p = PROMPT_MARK;
		else if	(mode_depth)	p = PROMPT_SUB;
		else if	(prompt)	p = prompt;
		else			p = PROMPT_STD;

		line++;
		io_psub(iostd, p);
	}

	c = getchar();

	if	(c == EOF && !batchmode)
	{
		ia_put('\n', ptr);
	}
	else	iflag = (c == '\n');

	return c;
}


/*	Zeichen nach Standardausgabe schreiben
*/

static int ia_put(int c, void *ptr)
{
	putchar(c);
	return 1;
}


/*	Kontrollfunktion
*/

static int ia_ctrl(void *par, int req, va_list list)
{
	char **ptr;

	switch (req)
	{
	case IO_LINEMARK:
		
		mark = 1;
		return 0;

	case IO_PROTECT:
	case IO_SUBMODE:
	
		if	(va_arg(list, int))	mode_depth++;
		else if	(mode_depth > 0)	mode_depth--;

		return mode_depth;

	case IO_IDENT:

		*va_arg(list, char **) = msprintf("<interact>:%d",
			line ? line : 1);
		return 0;

	case IO_PROMPT:

		if	((ptr = va_arg(list, char **)) != NULL)
		{
			char *save = prompt;
			prompt = *ptr;
			*ptr = save;
			return 0;
		}
		
		return EOF;

	case IO_LINE:

		return (line ? line : 1);

	case IO_ERROR:

		return batchmode ? EOF : 0;

	default:

		break;
	}

	return EOF;
}


io_t *(*_interact_open) (const char *prompt, const char *hist) = NULL;
io_t *(*_interact_filter) (io_t *io) = NULL;

io_t *io_interact (const char *prompt, const char *hist)
{
	io_t *io;

	if	(!_interact_open)
	{
		io = io_alloc();
		io->get = ia_get;
		io->put = ia_put;
		io->ctrl = ia_ctrl;
		prompt = mstrcpy(prompt);
	}
	else	io = _interact_open(prompt, hist);

	return _interact_filter ? _interact_filter(io) : io;
}
