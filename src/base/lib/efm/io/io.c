/*	Verwaltung der IO-Strukturen
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/


#include <EFEU/mstring.h>
#include <EFEU/io.h>
#include <EFEU/ioctrl.h>

static ALLOCTAB(io_tab, 0, sizeof(io_t));

int io_close_stat = 0;

static io_t *io_admin (io_t *tg, const io_t *src)
{
	if	(tg)
	{
		io_pushback(tg);
		io_close_stat = io_ctrl(tg, IO_CLOSE);

		if	(src == NULL)
		{
			del_data(&io_tab, tg);
			tg = NULL;
		}
	}
	else	tg = src ? (io_t *) src : new_data(&io_tab);

	return tg;
}

REFTYPE(io_reftype, "IO", io_ident, io_admin);


/*	IO-Struktur generieren
*/

io_t *io_alloc (void)
{
	return rd_create(&io_reftype);
}


/*	IO-Struktur schließen/Referenzzähler verringern
*/

int io_close (io_t *io)
{
	io_close_stat = 0;
	rd_deref(io);
	return io_close_stat;
}


/*	Gepufferte Zeichen zurückschreiben
*/

int io_pushback (io_t *io)
{
	if	(io == NULL)	return 0;

	if	(io->nsave)
	{
		int i;

		for (i = 0; i < io->nsave; i++)
			if (io_ctrl(io, IO_UNGETC, io->save_buf[i]) == EOF)
				return EOF;

		io->nsave = 0;
	}

	if	(io->stat == IO_STAT_EOF)
		io->stat = IO_STAT_OK;

	return 0;
}


/*	Identität abfragen
*/

char *io_ident (io_t *io)
{
	char *name;

	if	(io_ctrl(io, IO_IDENT, &name) != EOF)
		return name;

	return io ? mstrcpy("<io>") : NULL;
}


/*	Zurücksetzen der IO-Struktur
*/

int io_rewind (io_t *io)
{
	if	(io_ctrl(io, IO_REWIND) == EOF)
		return EOF;

	io->nsave = 0;
	io->stat = 0;
	return 0;
}
