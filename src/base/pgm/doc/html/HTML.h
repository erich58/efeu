/*
HTML-Ausgabefilter

$Copyright (C) 2000 Erich Frühstück
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

#ifndef	HTML_h
#define	HTML_h	1

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/stack.h>
#include <DocDrv.h>

#define	HTML_CTRL	('h' << 8)
#define	HTML_TITLE	(HTML_CTRL|0x1)
#define	HTML_CHAP	(HTML_CTRL|0x2)
#define	HTML_SEC	(HTML_CTRL|0x3)


typedef struct {
	DOCDRV_VAR;	/* Standardausgabevariablen */
	int class;	/* Dokumentklasse */
	int copy;	/* Kopiermodus */
	StrBuf buf;	/* Temporärer Buffer */
	char *nextpar;	/* Absatzkennung */
	char *enditem;	/* Item - Endekemnnung */
	Stack *s_att;	/* Stack mit Attributen */
	char *att;	/* Aktuelles Attribute */
	Stack *s_cmd;	/* Befehlsstack */
	char *hmode;	/* Zeichenkette vor dem nächsten Zeichen */
	VecBuf colgrp;	/* Spaltendefinitionen */
	int cpos;	/* Spaltenposition */
} HTML;

typedef void (*HTMLCmd) (HTML *html, void *data); 

void HTMLCMD_note (HTML *html, void *data);
void HTML_cpush (HTML *html, HTMLCmd cmd, void *data);
void HTML_cpop (HTML *html);

IO *DocOut_html (IO *io);

void HTML_puts (HTML *html, const char *str);
void HTML_newline (HTML *html, int n);
void HTML_hmode (HTML *html);

void HTML_sym (void *drv, const char *sym);
int HTML_putc (void *drv, int c);
int HTML_plain (void *drv, int c);
void HTML_rem (void *drv, const char *rem);
int HTML_cmd (void *drv, va_list list);
int HTML_env (void *drv, int flag, va_list list);

IO *html_open (const char *dir, const char *path);
char *HTML_colgrp (HTML *html);
void HTML_coldef (HTML *html, const char *def);

#endif	/* HTML.h */
