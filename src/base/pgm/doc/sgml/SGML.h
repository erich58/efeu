/*
SGML-Ausgabefilter

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef	_EFEU_SGML_h
#define	_EFEU_SGML_h	1

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/stack.h>
#include <DocDrv.h>


typedef struct {
	DOCDRV_VAR;	/* Standardausgabevariablen */
	int class;	/* Dokumentklasse */
	int copy;	/* Kopiermodus */
	char *nextpar;	/* Absatzkennung */
	stack_t *s_att;	/* Stack mit Attributen */
	char *att;	/* Aktuelles Attribute */
} SGML_t;

extern io_t *DocOut_sgml (io_t *io);

extern int SGML_putc (SGML_t *sgml, int c);
extern int SGML_plain (SGML_t *sgml, int c);
extern void SGML_puts (SGML_t *sgml, const char *str);
extern void SGML_newline (SGML_t *sgml, int n);
extern void SGML_rem (SGML_t *sgml, const char *rem);
extern void SGML_sym (SGML_t *sgml, const char *sym);
extern int SGML_cmd (SGML_t *sgml, va_list list);
extern int SGML_env (SGML_t *sgml, int flag, va_list list);

#endif	/* EFEU/SGML.h */
