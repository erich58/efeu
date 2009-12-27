/*	Eingabestruktur
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/EisDoc.h>
#include <ctype.h>

static ALLOCTAB(input_tab, 32, sizeof(EisDocInput_t));

void EisDocInput_push (EisDocInput_t **input, io_t *io)
{
	EisDocInput_t *x = new_data(&input_tab);

	if	(*input)
	{
		*x = **input;
		x->prev = *input;
	}
	else
	{
		memset(x, 0, sizeof(EisDocInput_t));
		x->comkey = 0;
		x->mergekey = '\\';
	}

	if	(io)
	{
		x->io = io;
	}
	else	rd_refer(x->io);

	*input = x;
}

void EisDocInput_pop (EisDocInput_t **input)
{
	if	(*input)
	{
		EisDocInput_t *x = *input;
		*input = x->prev;
		rd_deref(x->io);
		del_data(&input_tab, x);
	}
}

int EisDocInput_getc (EisDocInput_t *input)
{
	if	(input)
	{
		int c;

		while ((c = io_getc(input->io)) != EOF)
		{
			switch (c)
			{
			case '\n':
			case ' ':
				return c;
			case '\t':
				return input->obeytab ? '\t' : ' ';
			default:
				break;
			}

			/*
			if	(!iscntrl(c))	return c;
			*/
			return c;
		}
	}
	
	return EOF;
}

void EisDocInput_ungetc (EisDocInput_t *input, int c)
{
	if	(input)	io_ungetc(c, input->io);
}

int EisDocInput_peek (EisDocInput_t *input)
{
	int c = EisDocInput_getc(input);
	EisDocInput_ungetc(input, c);
	return c;
}

void EisDocInput_tmpcpy (EisDocInput_t **input, io_t *tmp)
{
	io_rewind(tmp);
	EisDocInput_push(input, tmp);
	(*input)->comkey = 0;
	(*input)->mergekey = 0;
	(*input)->evalmode = 0;
}
