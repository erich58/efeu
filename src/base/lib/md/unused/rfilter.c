/*	Lesefilter für mdread
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/mdlabel.h>


/*	Datenstrukturen
*/

typedef struct {
	io_t *io;	/* Eingabestruktur */
	int mode;	/* Verarbeitungsmodus */
	int sflag;	/* Stringflag */
	int delim;	/* Aktuelles Trennzeichen */
	int ndelim;	/* Zahl der gespeicherte Trennzeichen */
	char *skip;	/* Skipmuster */
	char *ignore;	/* Zu ignorierende Zeichen in Zahlenwerten */
} RFILTER;


static int if_get (RFILTER *filter);
static int if_mark (RFILTER *filter, int c);
static int if_ctrl (RFILTER *filter, int req, va_list list);


/*	Eingabefilter generieren
*/

io_t *rfilter(io_t *io)
{
	RFILTER *filter;

	filter = memalloc(sizeof(RFILTER));
	filter->io = io_lnum(io);
	filter->mode = ICTRL_HEAD;
	filter->sflag = 0;
	filter->delim = 0;
	filter->ndelim = 0;
	filter->skip = "%s";
	filter->ignore = NULL;

	io = io_alloc();
	io->get = (io_get_t) if_get;
	io->ctrl = (io_ctrl_t) if_ctrl;
	io->data = filter;
	return io;
}


/*	Zeichen markieren
*/

static int if_mark(RFILTER *filter, int c)
{
	if	(filter->mode != ICTRL_HEAD)
	{
		io_ungetc(c, filter->io);
		return '\\';
	}
	else	return c;
}


/*	Zeichen lesen
*/

static int if_get(RFILTER *filter)
{
	int c, d;

	if	(filter->ndelim)
	{
		filter->ndelim--;
		return filter->delim;
	}

	if	(filter->skip)
	{
		io_eat(filter->io, filter->skip);
		filter->skip = NULL;
	}

	while ((c = io_getc(filter->io)) != EOF)
	{
		switch (c)
		{
		case '/':

			if	(filter->sflag || iocpy_cskip(filter->io,
					NULL, c, NULL, 1) == EOF)
			{
				return c;
			}
			else	break;

		case '"':

			if	(!filter->sflag)
			{
				filter->sflag = 1;
			}
			else if	((d = io_getc(filter->io)) == '"')
			{
				return '"';
			}
			else
			{
				io_ungetc(d, filter->io);
				filter->sflag = 0;
			}

			break;

		case '\\':
			
			do	c = io_getc(filter->io);
			while	(c == '\r');

			switch (c)
			{
			case ' ':
			case '\t':
			case ':':
			case ';':	return if_mark(filter, c);
			case '\n':	break;
			default:	return c;
			}

			break;

		case '~':

			return if_mark(filter, ' ');

		case '\n':

			if	(filter->sflag)
			{
				return if_mark(filter, ' ');
			}

			filter->skip = "%s";
			return c;

		case ' ':
		case '\t':

			if	(filter->sflag)
			{
				return if_mark(filter, c);
			}

			switch (io_eat(filter->io, " \t"))
			{
			case ':':
			case ';':
			case '\n':	break;
			default:	return c;
			}

			break;

		case ':':
		case ';':

			if	(filter->sflag)
			{
				return if_mark(filter, c);
			}

			while ((d = io_eat(filter->io, " \t\r")) == c)
			{
				io_getc(filter->io);
				filter->ndelim++;
			}

			if	(d == '\n')
			{
				filter->ndelim = 0;
				filter->skip = "%s";
				return d;
			}

			filter->delim = c;
			return c;

		case ',':
		
			return (filter->mode == ICTRL_NUM ? '.' : c);

		case '\r':
		case '\f':

			break;

		default:

			if	(filter->mode != ICTRL_NUM ||
				!listcmp(filter->ignore, c))
			{
				return c;
			}

			break;
		}
	}

	return c;
}


/*	Kontrollfunktion
*/

static int if_ctrl(RFILTER *filter, int req, va_list list)
{
	char *p;
	int n;
	io_t *save;

	switch (req)
	{
	case IO_CLOSE:

		n = io_close(filter->io);
		memfree(filter);
		return n;

	case IO_ERROR:

		return 0;

	case ICTRL_NUM:
	case ICTRL_TEXT:
	case ICTRL_HEAD:

		filter->mode = req;
		return 0;

	case ICTRL_IGNORE:

		filter->ignore = mstrcpy(va_arg(list, char *));
		return 0;

	default:

		break;
	}

	if	(filter->io->ctrl)
	{
		return filter->io->ctrl(filter->io->data, req, list);
	}

	return EOF;
}
