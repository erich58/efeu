/*	term-Ausgabefilter
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.4
*/

#ifndef	_EFEU_term_h
#define	_EFEU_term_h	1

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/stack.h>
#include <EFEU/vecbuf.h>
#include <EFEU/object.h>
#include <DocDrv.h>

typedef struct {
	int wpmargin;	/* Umbruchspalte */
	char *rm;	/* Wechsel zu Roman */
	char *it;	/* Wechsel zu Italic */
	char *tt;	/* Wechsel zu Schreibmascinenschrift */
	char *bf;	/* Wechsel zu Fettschrift */
} TermPar_t;

#define	TERM_INDENT	4	/* Einrücktiefe */

extern TermPar_t TermPar;
void TermPar_init(void);

typedef struct {
	DOCDRV_VAR;		/* Standardausgabevariablen */
	unsigned margin;	/* Linker Rand */
	unsigned col;		/* Aktuelle Spalte */
	unsigned space;		/* Leerzeichen ausgeben */
	unsigned mode;		/* Leerzeichenmodus */
	unsigned hangpar;	/* Hängende Absätze */
	unsigned hang;		/* Einrücktiefe nach Umbruch */
	stack_t *stack;		/* Stack für Umgebungasparameter */
	stack_t *s_att;		/* Stack mit Attributen */
	char *att;		/* Aktuelles Attribut */
} term_t;

extern io_t *DocOut_term (io_t *io);

extern void term_att (term_t *trm, int flag, char *att);
extern int term_putc (term_t *mr, int c);
extern int term_verb (term_t *mr, int c);
extern void term_string (term_t *mr, const char *str);
extern void term_newline (term_t *mr, int flag);
extern void term_hmode (term_t *mr);
extern int term_cmd (term_t *mr, va_list list);
extern int term_env (term_t *mr, int flag, va_list list);

#endif	/* EFEU/term.h */
