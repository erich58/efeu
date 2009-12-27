/*	SynTeX-Ausgabefilter
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.4
*/

#ifndef	_EFEU_SynTeX_h
#define	_EFEU_SynTeX_h	1

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/stack.h>
#include <EFEU/vecbuf.h>
#include <EFEU/object.h>
#include <DocDrv.h>


typedef struct {
	DOCDRV_VAR;		/* Standardausgabevariablen */
	int ignorespace;	/* Leerzeichen ignorieren */
	int space;		/* Leerzeichen vor Text */
	int hang;		/* Hängende Absätze */
} SynTeX_t;

extern io_t *DocOut_syntex (io_t *io);

extern int SynTeX_putc (SynTeX_t *stex, int c);
extern int SynTeX_xputc (SynTeX_t *stex, int c);
extern int SynTeX_plain (SynTeX_t *stex, int c);
extern void SynTeX_puts (SynTeX_t *stex, const char *str);
extern void SynTeX_iputs (SynTeX_t *stex, const char *str);
extern void SynTeX_newline (SynTeX_t *stex);
extern void SynTeX_rem (SynTeX_t *stex, const char *rem);
extern int SynTeX_cmd (SynTeX_t *stex, va_list list);
extern int SynTeX_env (SynTeX_t *stex, int flag, va_list list);

#endif	/* EFEU/SynTeX.h */
