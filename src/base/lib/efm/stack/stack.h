/*	Stackstruktur
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4

$Header	<EFEU/$1>
*/

#ifndef	EFEU_STACK_H
#define	EFEU_STACK_H	1

#include <EFEU/config.h>

typedef struct stack_s stack_t;

struct stack_s {
	stack_t *next;	/* Verweis auf Nachfolger */
	void *data;	/* Eintragung */
};

void pushstack (stack_t **ptr, void *data);
void *popstack (stack_t **ptr, void *defval);

#define	stackdata(x, def)	((x) ? (x)->data : def)

#endif	/* EFEU/stack.h */
