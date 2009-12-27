/*	Eingabekonverter
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/InputEnc.h>
#include <EFEU/strbuf.h>
#include <EFEU/ioctrl.h>

#define	TODO	0

#define	BSIZE	32	/* Blockgröße für Zwischenbuffer */

/*	Eingabestruktur
*/

typedef struct {
	io_t *io;		/* Basiseingabe */
	uchar_t *ptr;		/* Zeichenpointer */
	InputEnc_t *enc;	/* Konvertierungstabelle */
	strbuf_t *buf;		/* Zeichenbuffer */
	int protect;		/* Schutzflag */
} ENCPAR;

static char *load_buf (ENCPAR *par, int c)
{
	InputEncEntry_t *tab;
	strbuf_t *sb;
	int n, n0, i0, i1, i2;

	tab = par->enc->tab.data;
	sb = par->buf;
	n = 0;
	i1 = 0;
	i2 = (int) par->enc->tab.used - 1;
	i0 = EOF;
	n0 = 1;

#if	TODO
	while (c != EOF)
	{
		c = (uchar_t) c;

	/*	Bereich bestimmen
	*/
		while (i1 < i2)
		{
			int i = (i1 + i2) / 2;

			if	(tab[i].name[n] < c)	i1 = i + 1;
			else if	(tab[i].name[n] > c)	i2 = i - 1;
			else				break;
		}

		while (i2 >= i1 && tab[i2].name[n] > c)
			i2--;

		while (i1 <= i2 && tab[i1].name[n] < c)
			i1++;

	/*	Zeichen buffern
	*/
		if	(n >= par->size)
			expand_buf(par);

		par->buf[n++] = c;

		if	(i1 > i2)	break;

	/*	Übereinstimmung speichern
	*/
		if	(tab[i1].name[n] == 0)
		{
			if	(i1 < i2)
			{
				n0 = n;
				i0 = i1;
				i1++;
			}
			else	return tab[i1].value;
		}

		c = io_getc(par->io);
	}

	if	(n == 0)	return NULL;

/*	Reduktion auf letzte Übereinstimmung
*/
	while (n > n0)
	{
		n--;
		io_ungetc(par->buf[n], par->io);
	}

	if	(i0 == EOF)
	{
		if	(n >= par->size)
			expand_buf(par);

		par->buf[n] = 0;
		return par->buf;
	}
	else	return tab[i0].value;
#else
	sb_putc(c, sb);
	return NULL;
#endif
}

/*	Zeichen lesen
*/

static int enc_get (ENCPAR *par)
{
	while (par->ptr == NULL || *par->ptr == 0)
	{
		int c = io_getc(par->io);

		if	(par->protect)
		{
			if	(par->buf->pos)
				return sb_delete(par->buf);

			return c;
		}

		par->ptr = load_buf(par, c);

		if	(par->ptr)	continue;
		else if	(par->buf->pos)	return sb_delete(par->buf);
		else			return c;
	}

	return *(par->ptr++);
}


/*	Kontrollfunktion
*/

static int enc_ctrl (ENCPAR *par, int req, va_list list)
{
	int stat;

	switch (req)
	{
	case IO_CLOSE:

		if	(par->buf->pos && par->io->refcount > 1)
		{
			io_push(par->io, io_mstr(sb2str(par->buf)));
		}
		else	del_strbuf(par->buf);

		rd_deref(par->enc);
		stat = io_close(par->io);
		memfree(par);
		return stat;

	case IO_REWIND:

		if	(io_rewind(par->io) == EOF)	return EOF;

		par->protect = 0;
		par->ptr = NULL;
		return 0;

	case IO_IDENT:
		*va_arg(list, char **) = io_xident(par->io, "inputenc(%*)");
		return 0;
		
	case IO_PROTECT:

		if	(va_arg(list, int))	par->protect++;
		else if	(par->protect > 0)	par->protect--;

		return par->protect;

	default:

		return io_vctrl (par->io, req, list);
	}
}


io_t *io_InputEnc (io_t *io, InputEnc_t *enc)
{
	if	(io && enc)
	{
		ENCPAR *par = memalloc(sizeof(ENCPAR));
		par->io = io;
		par->enc = enc;
		par->buf = new_strbuf(BSIZE);
		io = io_alloc();
		io->get = (io_get_t) enc_get;
		io->ctrl = (io_ctrl_t) enc_ctrl;
		io->data = par;
	}
	else	rd_deref(enc);

	return io;
}
