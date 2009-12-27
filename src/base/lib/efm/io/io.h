/*	IO-Definitionen für EFEU-Programmbibliothek
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4

$Header	<EFEU/$1>
*/

#ifndef	EFEU_IO_H
#define	EFEU_IO_H	1

#include <EFEU/memalloc.h>
#include <EFEU/refdata.h>
#include <EFEU/strbuf.h>


/*	IO-Struktur
*/

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

#define	COMPLEX_IODATA(get, put, ctrl, dbread, dbwrite, data)	\
{ REFDATA(&io_reftype), get, put, ctrl, dbread, dbwrite, data, 0, 0, { 0 }}

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


/*	Makros und Kontrollfunktionen
*/

#define	io_putchar(c)	io_putc(c, iostd)
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

#define	io_refer(io)	rd_refer(io)


/*	Initialisierungsfunktionen
*/

extern io_t *io_string (char *str, clean_t clean);
extern io_t *io_strbuf (strbuf_t *buf);
extern io_t *io_tmpbuf (size_t size);
extern io_t *io_bigbuf (size_t size, const char *pfx);

#define	io_cstr(str)	io_string((char *) (str), NULL)
#define	io_mstr(str)	io_string((char *) (str), memfree)

extern io_t *io_stream (const char *name,
	FILE *file, int (*close) (FILE *file));
extern io_t *io_fopen (const char *name, const char *mode);
extern io_t *io_fileopen (const char *name, const char *mode);
extern io_t *io_findopen (const char *path, const char *name,
	const char *type, const char *mode);
extern io_t *io_tmpfile (void);
extern io_t *io_popen (const char *name, const char *mode);


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


/*	IO-Stack
*/

extern io_t *iostack (io_t *io);
extern void iostack_push (io_t *base, io_t *io);
extern io_t *iostack_pop (io_t *base);
extern io_t *iostack_get (io_t *base);
extern void iostack_protect (io_t *base, int flag);

#endif	/* EFEU/io.h */
