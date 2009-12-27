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

#ifndef	EFEU_io_h
#define	EFEU_io_h	1

#include <EFEU/memalloc.h>
#include <EFEU/refdata.h>
#include <EFEU/strbuf.h>
#include <EFEU/stdint.h>
#include <EFEU/StrPool.h>
#include <EFEU/LogConfig.h>

#define	IO_MAX_SAVE	6
#define	IO_STAT_OK	0
#define	IO_STAT_EOF	1
#define	IO_STAT_ERR	2

extern RefType io_reftype;

typedef struct IO IO;

struct IO {
	REFVAR;
	int (*get) (IO *io);
	int (*put) (int c, IO *io);
	int (*ctrl) (IO *io, int c, va_list list);
	size_t (*dbread) (IO *io, void *data,
		size_t recl, size_t size, size_t dim);
	size_t (*dbwrite) (IO *io, const void *data,
		size_t recl, size_t size, size_t dim);
	void *data;

	unsigned char stat;
	unsigned char nsave;
	unsigned char save_buf[IO_MAX_SAVE];

	int32_t (*getucs) (IO *io);
	int (*putucs) (int32_t c, IO *io);
	uint32_t ucs_save : 1;
	uint32_t ucs_char : 31;
};

/*
Der Makro |$1| dient zur Initialisierung einer komplexen IO-Struktur
*/

#define	COMPLEX_IODATA(get, put, ctrl, dbread, dbwrite, data)	\
{ REFDATA(&io_reftype), get, put, ctrl, dbread, dbwrite, data, 0, 0, { 0 }, \
NULL, NULL, 0, 0 }

/*
Der Makro |$1| dient zur Initialisierung einer einfachen IO-Struktur
*/

#define	STD_IODATA(get,put,ctrl,par)	\
COMPLEX_IODATA(get,put,ctrl,NULL,NULL,par)

/*	Basisfunktionen
*/

IO *io_alloc (void);
int io_getc (IO *io);
int io_putc (int c, IO *io);
int io_ctrl (IO *io, int req, ...);
int io_vctrl (IO *io, int req, va_list list);
int io_peek (IO *io);
int io_copy (IO *in, IO *out);

/*
Der Makro |$1| schreibt ein Zeichen nach |iostd|.
*/

#define	io_putchar(c)	io_putc(c, iostd)

/*
Der Makro |$1| liest ein Zeichen aus |iostd|.
*/

#define	io_getchar()	io_getc(iostd)

char *io_xident (IO *io, const char *fmt, ...);

int io_ungetc (int c, IO *io);
int io_pushback (IO *io);
int io_rewind (IO *io);
int io_close (IO *io);
int io_err (IO *io);

int io_close_stat;

void io_push (IO *io, IO *tmp);
IO *io_pop (IO *io);
IO *io_divide (IO *io, size_t bsize);

void io_submode (IO *io, int flag);
void io_protect (IO *io, int flag);
void io_linemark (IO *io);
char *io_prompt (IO *io, const char *prompt);

/*
Der Makro |$1| erhönht den Referenzzähler der IO-Struktur <io>
*/

#define	io_refer(io)	rd_refer(io)

/*	Initialisierungsfunktionen
*/

IO *io_string (char *str, void (*clean) (void *ptr));
IO *io_strbuf (StrBuf *buf);
IO *io_tmpbuf (size_t size);
IO *io_bigbuf (size_t size, const char *pfx);
IO *io_data (void *ref, void *data, size_t size);

IO *StrPool_open (StrPool *pool);
void StrPool_dump (StrPool *pool, IO *out);
size_t StrPool_read (StrPool *pool, size_t size, IO *in);
size_t StrPool_write (StrPool *pool, IO *out);

/*
Der Makro |$1| liefert eine IO-Struktur auf einen konstanten String.
*/

#define	io_cstr(str)	io_string((char *) (str), NULL)

/*
Der Makro |$1| liefert eine IO-Struktur auf einen dynamischen String.
Dieser wird beim schließen der IO-Struktur automatisch freigegeben.
*/

#define	io_mstr(str)	io_string((char *) (str), memfree)

IO *io_stream (const char *name, FILE *file, int (*close) (FILE *file));
IO *io_fopen (const char *name, const char *mode);
IO *io_fileopen (const char *name, const char *mode);
IO *io_findopen (const char *path, const char *name,
	const char *type, const char *mode);
IO *io_tmpfile (void);
IO *io_popen (const char *name, const char *mode);
IO *io_interact (const char *prompt, const char *hist);
IO *io_batch (void);

IO *(*_interact_open) (const char *prompt, const char *hist);
IO *(*_interact_filter) (IO *io);


/*	Eingabefunktionen
*/

int io_skipcom (IO *io, StrBuf *buf, int flag);
int io_eat (IO *io, const char *wmark);
int io_skip (IO *io, const char *delim);
int io_mgetc (IO *io, int flag);

char *io_gets (char *s, int n, IO *io);
char *io_mgets (IO *io, const char *delim);
int io_mcopy (IO *in, IO *out, const char *delim);

int io_xgetc (IO *io, const char *delim);
char *io_xgets (IO *io, const char *delim);
int io_xcopy (IO *in, IO *out, const char *delim);

char *getstring (IO *io);


/*	Ausgaberoutinen
*/

int io_nputc (int c, IO *io, int n);
int io_mputc (int c, IO *io, const char *delim);
int io_xputc (int c, IO *io, const char *delim);
int io_puts (const char *s, IO *io);
int io_nlputs (const char *s, IO *io);
int io_mputs (const char *s, IO *io, const char *delim);
int io_xputs (const char *s, IO *io, const char *delim);
int io_langputs (const char *s, IO *io);

int io_vprintf (IO *io, const char *fmt, va_list list);
int io_vxprintf (IO *io, const char *fmt, va_list list);
int io_vmbprintf (IO *io, const char *fmt, va_list list);

int io_printf (IO *io, const char *fmt, ...);
int io_xprintf (IO *io, const char *fmt, ...);
int io_mbprintf (IO *io, const char *fmt, ...);

void log_vprintf (LogControl *log, const char *fmt, va_list list);
void log_printf (LogControl *log, const char *fmt, ...);

/*	Binäre Ein/Ausgabe
*/

size_t io_read (IO *io, void *buf, size_t nbyte);
size_t io_rread (IO *io, void *buf, size_t nbyte);
size_t io_write (IO *io, const void *buf, size_t nbyte);
size_t io_rwrite (IO *io, const void *buf, size_t nbyte);
char *io_mread (IO *io, size_t nbyte);
char *io_getarg (IO *io, int beg, int end);

size_t io_dbread (IO *io, void *data,
	size_t recl, size_t size, size_t dim);
size_t io_dbwrite (IO *io, const void *data,
	size_t recl, size_t size, size_t dim);

char *io_getstr (IO *io);
int io_putstr (const char *str, IO *io);

unsigned io_getval (IO *io, int n);
void io_putval (unsigned val, IO *io, int n);
int64_t io_llget (IO *io, size_t recl);
void io_llput (int64_t val, IO *io, size_t recl);
uint64_t io_ullget (IO *io, size_t recl);
void io_ullput (uint64_t val, IO *io, size_t recl);

size_t io_llread (IO *io, int64_t *data, size_t dim);
size_t io_llwrite (IO *io, const int64_t *data, size_t dim);
size_t io_ullread (IO *io, uint64_t *data, size_t dim);
size_t io_ullwrite (IO *io, const uint64_t *data, size_t dim);

size_t io_write_size (size_t val, IO *io);
size_t io_read_size (size_t *buf, IO *io);

char *io_getline (IO *io, StrBuf *buf, int delim);
char *io_mgetline (IO *io, StrBuf *buf, const char *delim);

/*
Unicode Hilfsfunktionen
*/

int32_t io_getucs (IO *io);
int32_t io_getucs_utf8 (IO *io);
int32_t io_getucs_latin1 (IO *io);
int32_t io_getucs_latin9 (IO *io);
int32_t io_ucscompose (IO *io, int c);
int32_t io_ungetucs (int32_t c, IO *io);

void io_ucswmode (IO *io, const char *codeset);

int io_putucs (int32_t c, IO *io);
int io_putucs_ascii (int32_t c, IO *io);
int io_putucs_latin1 (int32_t c, IO *io);
int io_putucs_latin9 (int32_t c, IO *io);
int io_putucs_xml (int32_t c, IO *io);
int io_putucs_utf8 (int32_t c, IO *io);

IO *io_ucs2latin9 (IO *base);

/*	Fehlermeldungen
*/

void io_note (IO *io, const char *fmt, const char *def, ...);
void io_error (IO *io, const char *fmt, const char *def, ...);


/*	Standard Ein-Ausgabestrukturen
*/

extern IO *iostd;		/* Standardein/ausgabe */
extern IO *iomsg;		/* Ausgabe für message */
extern IO *ioerr;		/* Standardfehler */
extern IO *ionull;		/* Null-Device */


/*	Filterprogramme
*/

IO *io_lnum (IO *io);	/* Zeilennummer */
IO *io_count (IO *io);	/* Ausgabezähler */
IO *io_html (IO *io);	/* HTML-Filter */
IO *io_crlf (IO *io);	/* CRLF Filter */
IO *io_indent (IO *io, int c, int n);
IO *io_lmark (IO *io, const char *pre, const char *post, int flag);
IO *io_cleanup (IO *io, void (*cf) (IO *io, void *p), void *p);
IO *xlangfilter (IO *in, const char *lang,
	int (*newpar) (IO *io, int c, void *par), void *par);
IO *langfilter (IO *in, const char *lang);

IO *in_utf8 (IO *io);	/* UTF-8 Eingabekonvertierung */
IO *out_utf8 (IO *io);	/* UTF-8 Ausgabekonvertierung */
IO *out_latin9 (IO *io);	/* LATIN-9 Ausgabekonvertierung */

/*	Teilfileausgabe in Bibliothek
*/

IO *diropen (const char *name, const char *base);
int io_newpart (IO *io, const char *name, const char *repl);
int io_endpart (IO *io);

IO *LogOpen (LogControl *ctrl);

#endif	/* EFEU/io.h */
