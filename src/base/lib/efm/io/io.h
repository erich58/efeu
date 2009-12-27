/*
IO-Definitionen für EFEU-Programmbibliothek

$Header	<EFEU/$1>

$Copyright (C) 1997 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef	_EFEU_io_h
#define	_EFEU_io_h	1

#include <EFEU/memalloc.h>
#include <EFEU/refdata.h>
#include <EFEU/strbuf.h>

#define	IO_MAX_SAVE	6
#define	IO_STAT_OK	0
#define	IO_STAT_EOF	1
#define	IO_STAT_ERR	2

typedef int (*io_get_t) (void *par);
typedef int (*io_put_t) (int c, void *par);
typedef int (*io_ctrl_t) (void *par, int c, va_list list);
typedef size_t (*io_data_t) (void *par, void *data,
	size_t recl, size_t size, size_t dim);

extern reftype_t io_reftype;

typedef struct {
	REFVAR;
	io_get_t get;
	io_put_t put;
	io_ctrl_t ctrl;
	io_data_t dbread;
	io_data_t dbwrite;
	void *data;
	uchar_t stat;
	uchar_t nsave;
	uchar_t save_buf[IO_MAX_SAVE];
} io_t;

/*
Der Makro |$1| dient zur Initialisierung einer komplexen IO-Struktur
*/

#define	COMPLEX_IODATA(get, put, ctrl, dbread, dbwrite, data)	\
{ REFDATA(&io_reftype), get, put, ctrl, dbread, dbwrite, data, 0, 0, { 0 }}

/*
Der Makro |$1| dient zur Initialisierung einer einfachen IO-Struktur
*/

#define	STD_IODATA(get,put,ctrl,par)	\
COMPLEX_IODATA(get,put,ctrl,NULL,NULL,par)

/*	Basisfunktionen
*/

extern io_t *io_alloc (void);
extern int io_getc (io_t *io);
extern int io_putc (int c, io_t *io);
extern int io_ctrl (io_t *io, int req, ...);
extern int io_vctrl (io_t *io, int req, va_list list);
extern int io_peek (io_t *io);
extern int io_copy (io_t *in, io_t *out);


/*
Der Makro |$1| schreibt ein Zeichen nach |iostd|.
*/

#define	io_putchar(c)	io_putc(c, iostd)

/*
Der Makro |$1| liest ein Zeichen aus |iostd|.
*/

#define	io_getchar()	io_getc(iostd)

extern char *io_ident (io_t *io);
extern char *io_xident (io_t *io, const char *fmt, ...);

extern int io_ungetc (int c, io_t *io);
extern int io_pushback (io_t *io);
extern int io_rewind (io_t *io);
extern int io_close (io_t *io);

extern int io_close_stat;

extern void io_push (io_t *io, io_t *tmp);
extern io_t *io_pop (io_t *io);

extern void io_submode (io_t *io, int flag);
extern void io_protect (io_t *io, int flag);
extern void io_linemark (io_t *io);
extern char *io_prompt (io_t *io, const char *prompt);

/*
Der Makro |$1| erhönht den Referenzzähler der IO-Struktur <io>
*/

#define	io_refer(io)	rd_refer(io)

/*	Initialisierungsfunktionen
*/

extern io_t *io_string (char *str, clean_t clean);
extern io_t *io_strbuf (strbuf_t *buf);
extern io_t *io_tmpbuf (size_t size);
extern io_t *io_bigbuf (size_t size, const char *pfx);

/*
Der Makro |$1| liefert eine IO-Struktur auf einen konstanten String.
*/

#define	io_cstr(str)	io_string((char *) (str), NULL)

/*
Der Makro |$1| liefert eine IO-Struktur auf einen dynamischen String.
Dieser wird beim schließen der IO-Struktur automatisch freigegeben.
*/

#define	io_mstr(str)	io_string((char *) (str), memfree)

extern io_t *io_stream (const char *name,
	FILE *file, int (*close) (FILE *file));
extern io_t *io_fopen (const char *name, const char *mode);
extern io_t *io_fileopen (const char *name, const char *mode);
extern io_t *io_findopen (const char *path, const char *name,
	const char *type, const char *mode);
extern io_t *io_tmpfile (void);
extern io_t *io_popen (const char *name, const char *mode);
extern io_t *io_interact (const char *prompt, const char *hist);
extern io_t *io_batch (void);

extern io_t *(*_interact_open) (const char *prompt, const char *hist);
extern io_t *(*_interact_filter) (io_t *io);


/*	Eingabefunktionen
*/

extern int io_skipcom (io_t *io, strbuf_t *buf, int flag);
extern int io_eat (io_t *io, const char *wmark);
extern int io_skip (io_t *io, const char *delim);
extern int io_mgetc (io_t *io, int flag);

extern char *io_gets (char *s, int n, io_t *io);
extern char *io_mgets (io_t *io, const char *delim);
extern int io_mcopy (io_t *in, io_t *out, const char *delim);

extern int io_xgetc (io_t *io, const char *delim);
extern char *io_xgets (io_t *io, const char *delim);
extern int io_xcopy (io_t *in, io_t *out, const char *delim);

extern char *getstring (io_t *io);
extern int io_loadmsg (io_t *io, const char *name, const char *delim);


/*	Ausgaberoutinen
*/

extern int io_nputc (int c, io_t *io, int n);
extern int io_mputc (int c, io_t *io, const char *delim);
extern int io_xputc (int c, io_t *io, const char *delim);
extern int io_puts (const char *s, io_t *io);
extern int io_nlputs (const char *s, io_t *io);
extern int io_mputs (const char *s, io_t *io, const char *delim);
extern int io_xputs (const char *s, io_t *io, const char *delim);
extern int io_langputs (const char *s, io_t *io);

extern int io_vprintf (io_t *io, const char *fmt, va_list list);
extern int io_printf (io_t *io, const char *fmt, ...);

/*	Binäre Ein/Ausgabe
*/

extern size_t io_read (io_t *io, void *buf, size_t nbyte);
extern size_t io_rread (io_t *io, void *buf, size_t nbyte);
extern size_t io_write (io_t *io, const void *buf, size_t nbyte);
extern size_t io_rwrite (io_t *io, const void *buf, size_t nbyte);
extern char *io_mread (io_t *io, size_t nbyte);

extern size_t io_dbread (io_t *io, void *dp, size_t rl, size_t size, size_t n);
extern size_t io_dbwrite (io_t *io, void *dp, size_t rl, size_t size, size_t n);

typedef size_t (*io_dbdata_t) (io_t *io, void *data);

extern unsigned io_getval (io_t *io, int n);
extern char *io_getstr (io_t *io);

extern void io_putval (unsigned val, io_t *io, int n);
extern int io_putstr(const char *str, io_t *io);

extern size_t io_getsize (io_t *io);
extern int io_putsize (size_t val, io_t *io);

/*	Fehlermeldungen
*/

extern void io_message (io_t *io, const char *name, int num, int narg, ...);
extern void io_error (io_t *io, const char *name, int num, int narg, ...);


/*	Standard Ein-Ausgabestrukturen
*/

extern io_t *iostd;		/* Standardein/ausgabe */
extern io_t *iomsg;		/* Ausgabe für message */
extern io_t *ioerr;		/* Standardfehler */
extern io_t *ionull;		/* Null-Device */


/*	Filterprogramme
*/

extern io_t *io_lnum (io_t *io);	/* Zeilennummer */
extern io_t *io_count (io_t *io);	/* Ausgabezähler */
extern io_t *io_html (io_t *io);	/* HTML-Filter */
extern io_t *io_indent (io_t *io, int c, int n);
extern io_t *io_lmark (io_t *io, const char *pre, const char *post, int flag);
extern io_t *io_cleanup (io_t *io, void (*cf) (io_t *io, void *p), void *p);
extern io_t *langfilter (io_t *in, const char *lang);

/*	Teilfileausgabe in Bibliothek
*/

extern io_t *diropen(const char *name, const char *base);
extern int io_newpart(io_t *io, const char *name, const char *repl);
extern int io_endpart(io_t *io);

#endif	/* EFEU/io.h */
