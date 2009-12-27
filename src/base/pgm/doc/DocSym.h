/*	Symboltabellen
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.4
*/

#ifndef	_EFEU_DocSym_h
#define	_EFEU_DocSym_h	1

#include <EFEU/io.h>
#include <EFEU/Info.h>
#include <EFEU/strbuf.h>
#include <EFEU/stack.h>

#ifndef	MSG_DOC
#define	MSG_DOC	"Doc"
#endif


typedef struct {
	char *key;	/* Symbolkennung */
	char *fmt;	/* Symbolformat */
} DocSymEntry_t;

typedef struct {
	strbuf_t buf;	/* Stringbuffer */
	size_t dim;	/* Zahl der Einträge */
	DocSymEntry_t *tab;	/* Symbolvektor */
} DocSym_t;

DocSym_t *DocSym (const char *name);
int DocSym_print (io_t *io, DocSym_t *sym);
char *DocSym_get (DocSym_t *sym, const char *name);
void DocSym_free (DocSym_t *sym);

#endif	/* EFEU/DocSym.h */
