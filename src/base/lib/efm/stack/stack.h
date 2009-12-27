/*
:*:	data stack
:de:	Stackstruktur

$Header	<EFEU/$1>

$Copyright (C) 1998 Erich Frühstück
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

#ifndef	_EFEU_stack_h
#define	_EFEU_stack_h	1

#include <EFEU/config.h>

typedef struct stack_s stack_t;

struct stack_s {
	stack_t *next;	/* Verweis auf Nachfolger */
	void *data;	/* Eintragung */
};

extern void pushstack (stack_t **ptr, void *data);
extern void *popstack (stack_t **ptr, void *defval);

/*
:*:
The macro |$1| returns the value of the bottom element of the stack.
:de:
Der Makro |$1| liefert den Datenwert des untersten Stackelementes.
*/

#define	stackdata(x, def)	((x) ? (x)->data : def)

#endif	/* EFEU/stack.h */
