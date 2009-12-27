/*	Ein/Ausgabe auf String, Konvertierungsbuffer
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/


#include <EFEU/io.h>
#include <EFEU/ioctrl.h>

/*	Stringstruktur
*/

typedef struct {
	clean_t clean;
	uchar_t *str;
	unsigned pos;
} STRIO;

static STRIO *new_strio (char *str, clean_t clean)
{
	STRIO *s = memalloc(sizeof(STRIO));
	s->clean = clean;
	s->str = (uchar_t *) str;
	s->pos = 0;
	return s;
}

static void del_strio (STRIO *s)
{
	if	(s->clean)	s->clean(s->str);

	memfree(s);
}


/*	IO - Struktur
*/

static int str_get (STRIO *s)
{
	return s->str[s->pos] ? (int) s->str[s->pos++] : EOF;
}

static int str_ctrl (STRIO *s, int req, va_list list)
{
	switch (req)
	{
	case IO_CLOSE:	del_strio(s); return 0;
	case IO_REWIND:	s->pos = 0; return 0;
	case IO_IDENT:	*va_arg(list, char **) = "<string>"; return 0;
	default:	return EOF;
	}
}

io_t *io_string (char *str, clean_t clean)
{
	if	(str)
	{
		io_t *io = io_alloc();
		io->get = (io_get_t) str_get;
		io->ctrl = (io_ctrl_t) str_ctrl;
		io->data = new_strio(str, clean);
		return io;
	}

	return NULL;
}
