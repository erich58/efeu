/*	IO-Stackstruktur
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.4
*/

#include <EFEU/ioctrl.h>
#include <EFEU/stack.h>


typedef struct {
	stack_t *stack;
	int protect;
} STACKPAR;


/*	Zeichen lesen
*/

static int stack_get (STACKPAR *par)
{
	int c;

	while (par->stack != NULL)
	{
		c = io_getc(par->stack->data);

		if	(c == EOF && !par->protect)
			rd_deref(popstack(&par->stack, NULL));
		else	return c;
	}

	return EOF;
}

/*	Zeichen ausgeben
*/

static int stack_put (int c, STACKPAR *par)
{
	return par->stack ? io_putc(c, par->stack->data) : EOF;
}

/*	Identifikationsstring generieren
*/

static char *stack_ident (STACKPAR *par)
{
	strbuf_t *buf;
	stack_t *x;
	char *delim;

	buf = new_strbuf(0);
	delim = "{ ";

	for (x = par->stack; x != NULL; x = x->next)
	{
		char *p = io_ident(x->data);
		sb_puts(delim, buf);
		sb_puts(p, buf);
		memfree(p);
		delim = ", ";
	}

	sb_puts(" }", buf);
	return sb2str(buf);
}


/*	Kontrollfunktion
*/

static int stack_ctrl (STACKPAR *par, int req, va_list list)
{
	switch (req)
	{
	case IOSTACK_PROTECT:
		if	(va_arg(list, int))	par->protect++;
		else if	(par->protect)		par->protect--;

		return par->protect;
	case IOSTACK_PUSH:
		pushstack(&par->stack, va_arg(list, io_t *));
		return 0;
	case IOSTACK_POP:
		*va_arg(list, io_t **) = popstack(&par->stack, NULL);
		return 0;
	case IOSTACK_GET:
		*va_arg(list, io_t **) = stackdata(par->stack, NULL);
		return 0;
	case IO_IDENT:
		*va_arg(list, char **) = stack_ident(par);
		return 0;
	case IO_UNGETC:
		if	(par->stack)
			return io_ungetc(va_arg(list, int), par->stack->data);

		return EOF;
	case IO_CLOSE:
		while (par->stack != NULL)
			rd_deref(popstack(&par->stack, NULL));

		memfree(par);
		return 0;
	case IO_REWIND:	
		return EOF;
	default:
		break;
	}

	if	(par->stack)
		return io_vctrl(par->stack->data, req, list);

	return EOF;
}


/*	IO-Stack generieren
*/

io_t *iostack (io_t *io)
{
	STACKPAR *par = memalloc(sizeof(STACKPAR));

	if	(io)
		pushstack(&par->stack, io);

	io = io_alloc();
	io->get = (io_get_t) stack_get;
	io->put = (io_put_t) stack_put;
	io->ctrl = (io_ctrl_t) stack_ctrl;
	io->data = par;
	return io;
}

/*	Schnittstellenfunktionen
*/

void iostack_push (io_t *base, io_t *io)
{
	io_pushback(base);

	if	(io_ctrl(base, IOSTACK_PUSH, io) == EOF)
		io_close(io);
}

io_t *iostack_pop (io_t *base)
{
	io_t *io = NULL;
	io_pushback(base);

	if	(io_ctrl(base, IOSTACK_POP, &io) == EOF)
		return io_refer(base);

	return io;
}

io_t *iostack_get (io_t *base)
{
	io_t *io;
	io_pushback(base);

	if	(io_ctrl(base, IOSTACK_GET, &io) == EOF)
		io = base;

	return io_refer(io);
}

void iostack_protect (io_t *io, int val)
{
	io_protect(io, val);

	if (io_ctrl(io, IOSTACK_PROTECT, val) == 0 && io->stat == IO_STAT_EOF)
		io->stat = IO_STAT_OK;
}
