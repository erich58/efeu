/*	IO-Definitionen für efmain Programmbibliothek
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_EFIO_H
#define	EFEU_EFIO_H	1

#include <EFEU/efmain.h>
#include <EFEU/refdata.h>
#include <EFEU/io.h>
#include <EFEU/ioscan.h>


/*	Basisfunktionen
*/

io_t *io_alloc (void);
int io_getc (io_t *io);
int io_putc (int c, io_t *io);
int io_ctrl (io_t *io, int req, ...);
int io_vctrl (io_t *io, int req, va_list list);


/*	Makros und Kontrollfunktionen
*/

#define	io_putchar(c)	io_putc(c, iostd)
#define	io_getchar()	io_getc(iostd)

char *io_ident (io_t *io);
int io_ungetc (int c, io_t *io);
int io_pushback (io_t *io);
int io_rewind (io_t *io);
int io_close (io_t *io);


/*	Initialisierungsfunktionen
*/

io_t *io_cpystr (const char *buf);
io_t *io_str (const char *buf);
io_t *io_strbuf (strbuf_t *buf);
io_t *io_tmpbuf (size_t size);

io_t *io_stream (const char *name, FILE *file, int (*close) (FILE *file));
io_t *io_fopen (const char *name, const char *mode);
io_t *io_fileopen (const char *name, const char *mode);
io_t *io_findopen (const char *path, const char *name, const char *type, const char *mode);
io_t *io_tmpfile (void);
io_t *io_popen (const char *name, const char *mode);


/*	Eingabefunktionen
*/

int io_eat (io_t *io, const char *wmark);
int io_skip (io_t *io, const char *delim);
int io_mgetc (io_t *io, int flag);

char *io_gets (char *s, int n, io_t *io);
char *io_mgets (io_t *io, const char *delim);
int io_mcopy (io_t *in, io_t *out, const char *delim);

int io_xgetc (io_t *io, const char *delim);
char *io_xgets (io_t *io, const char *delim);
int io_xcopy (io_t *in, io_t *out, const char *delim);

char *getstring (io_t *io);
int io_loadmsg (io_t *io, const char *name, const char *delim);


/*	Ausgaberoutinen
*/

int io_nputc (int c, io_t *io, int n);
int io_mputc (int c, io_t *io, const char *delim);
int io_xputc (int c, io_t *io, const char *delim);
int io_puts (const char *s, io_t *io);
int io_mputs (const char *s, io_t *io, const char *delim);
int io_xputs (const char *s, io_t *io, const char *delim);

int io_vprintf (io_t *io, const char *fmt, va_list list);
int io_printf (io_t *io, const char *fmt, ...);


/*	Hilfsprogramme zur TeX - Ausgabe
*/

int TeXputs (const char *str, io_t *io);
int TeXquote (const char *str, io_t *io);
int TeXputc (int c, io_t *io);


/*	Binäre Ein/Ausgabe
*/

size_t io_read (io_t *io, void *buf, size_t nbyte);
size_t io_rread (io_t *io, void *buf, size_t nbyte);
size_t io_write (io_t *io, const void *buf, size_t nbyte);
size_t io_rwrite (io_t *io, const void *buf, size_t nbyte);
char *io_mread (io_t *io, size_t nbyte);

#if	REVBYTEORDER
#define	put_MSBF	io_rwrite
#define	put_LSBF	io_write
#define	get_MSBF	io_rread
#define	get_LSBF	io_read
#else
#define	put_MSBF	io_write
#define	put_LSBF	io_rwrite
#define	get_MSBF	io_read
#define	get_LSBF	io_rread
#endif


/*	Zwischenspeichern von Strings
*/

size_t io_split (io_t *io, const char *delim, char ***ptr);
size_t io_savestr (io_t *io, const char *str);
char *io_loadstr (io_t *io, char **ptr);


/*	Kopierfunktionen und Parametersubstitution
*/

#include <EFEU/parsub.h>


void io_eval (io_t *io, const char *delim);

#include <EFEU/iocpy.h>

int iocpy (io_t *in, io_t *out, iocpy_t *def, size_t dim);
char *miocpy (io_t *in, iocpy_t *def, size_t dim);
int iocpyfmt (const char *fmt, io_t *out, iocpy_t *def, size_t dim);
char *miocpyfmt (const char *fmt, iocpy_t *def, size_t dim);
io_t *cpyfilter (io_t *io, iocpy_t *def, size_t dim);

extern int iocpy_flag;	/* Flag für Sonderzeichen */
extern int iocpy_last;	/* Zuletzt gelesenes Zeichen */


/*	Standardkopierroutinen
*/

int iocpy_esc (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_mark (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_repl (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

int iocpy_skip (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_cskip (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

int iocpy_psub (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_eval (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_term (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

int iocpy_value (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_name (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_str (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_xstr (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

int iocpy_usage (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

extern xtab_t psub_tab;


/*	Zahlenwerte einlesen
*/

int io_scan (io_t *io, unsigned flags, void **ptr);
int io_valscan (io_t *io, unsigned flags, void **ptr);

#define	SCAN_INT	0x01	/* Normaler Ganzzahlwert */
#define	SCAN_LONG	0x02	/* Langer Ganzzahlwert */
#define	SCAN_DOUBLE	0x04	/* Gleitkommazahl */

#define	SCAN_CHAR	0x10	/* Zeichendefinition (einfache Anführung) */
#define	SCAN_STR	0x20	/* String (doppelte Anführung oder NULL) */
#define	SCAN_NAME	0x40	/* Name */
#define	SCAN_NULL	0x80	/* NULL - Key */

#define	SCAN_BINVAL	0x0100	/* Binärwert */
#define	SCAN_OCTVAL	0x0200	/* Oktalwert */
#define	SCAN_HEXVAL	0x0400	/* Hexadezimalwert */
#define	SCAN_UNSIGNED	0x1000	/* Vorzeichenfreier Wert */

#define	SCAN_VALMASK	0x000f	/* Maske für Zahlenwerte */
#define	SCAN_TYPEMASK	0x00ff	/* Maske für Werttype */
#define	SCAN_BASEMASK	0x0f00	/* Maske für Ziffernbasis */
#define	SCAN_MODEMASK	0xff00	/* Maske für Modifikationsflags */

#define	SCAN_INTEGER	0x1f03	/* Beliebiger Ganzzahlwert */
#define	SCAN_ANYVAL	0x1f07	/* Beliebiger Zahlenwert */
#define	SCAN_ANYTYPE	0x1fff	/* Beliebiger Type */


/*	Abfragefunktionen
*/

char *io_getname (io_t *io);
int io_testkey (io_t *io, const char *str);


/*	Standard Ein-Ausgabestrukturen
*/

extern io_t *iostd;		/* Standardein/ausgabe */
extern io_t *ioerr;		/* Standardfehler */
extern io_t *iomsg;		/* Ausgabe für message */


/*	Filterprogramme
*/

io_t *io_crfilter (io_t *io);
io_t *io_pgfmt (io_t *io);
io_t *io_termout (io_t *io, const char *term);


#endif	/* EFEU_EFIO_H */
