/*	Argumentliste generieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/preproc.h>
#include <EFEU/efutil.h>
#include <EFEU/vecbuf.h>

#define	PROMPT	"( >>> "

static iocpy_t copy_def[] = {
	{ "/", "\n", 1, iocpy_cskip },
	{ "\\", ",)", 0, iocpy_esc },
	{ "\\", "!", 1, iocpy_esc },
	{ "\"", "\"", 1, iocpy_str },
	{ "'",	"'", 1, iocpy_str },
	{ "(",	")", 1, iocpy_brace },
	{ "{",	"}", 1, iocpy_brace },
	{ "[",	"]", 1, iocpy_brace },
	{ ",)", NULL, 0, NULL },
	{ "%a_", "!%n_", 0, iocpy_macsub },
};


size_t macarglist(io_t *io, char ***ptr)
{
	vecbuf_t vb;
	char *prompt;
	char **p;
	int c;
	int dim;

	if	(ptr == NULL)
	{
		return 0;
	}
	else	*ptr = NULL;

	vb_init(&vb, 16, sizeof(char *));
	prompt = io_prompt(io, PROMPT);

/*	Argumente bestimmen
*/
	while ((c = io_eat(io, "%s")) != EOF)
	{
		p = vb_next(&vb);
		*p = NULL;

		if	(c != ',' && c != ')')
			*p = miocpy(io, copy_def, tabsize(copy_def));

		c = io_getc(io);

		if	(c == ')')	break;
	}

	io_prompt(io, prompt);

/*	Argumentliste übernehmen
*/
	dim = vb.used;
	*ptr = memalloc(dim * sizeof(char *));
	memcpy(*ptr, vb.data, dim * sizeof(char *));
	vb_free(&vb);
	return dim;
}
