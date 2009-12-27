/*	Stackstruktur
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4

$Header	<EFEU/$1>
*/

#ifndef	EFEU_OLDSTACK_H
#define	EFEU_OLDSTACK_H	1

#include <EFEU/config.h>
#include <EFEU/types.h>

typedef struct oldstack_s oldstack_t;

struct oldstack_s {
	oldstack_t *next;	/* Verweis auf Nachfolger */
	oldstack_t *prev;	/* Verweis auf Vorgänger */
	void *value;		/* Eintragung */
};

void pushstack (oldstack_t **stack, const void *value);
void topstack (oldstack_t **stack, const void *value);
void *popstack (oldstack_t **stack);
void rotstack (oldstack_t **stack, int direction);
void exchange (oldstack_t **stack);

#define	stackvalue(x)	((x) ? (x)->value : NULL)

void clrstack (oldstack_t **stack, void (*action) (void *));
int walkstack (oldstack_t *stack, visit_t visit);
int rwalkstack (oldstack_t *stack, visit_t visit);

#endif	/* EFEU/oldstack.h */
