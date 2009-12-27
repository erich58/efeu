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
	strbuf_t *buf;	/* Temporärer Buffer */
	char *nextpar;	/* Absatzkennung */
	char *enditem;	/* Item - Endekemnnung */
	stack_t *s_att;	/* Stack mit Attributen */
	char *att;	/* Aktuelles Attribute */
	int bline;	/* Flag für fette Tabellenzeile */
} HTML_t;

extern io_t *DocOut_html (io_t *io);

extern int HTML_putc (HTML_t *html, int c);
extern int HTML_plain (HTML_t *html, int c);
extern void HTML_puts (HTML_t *html, const char *str);
extern void HTML_newline (HTML_t *html, int n);
extern void HTML_rem (HTML_t *html, const char *rem);
extern void HTML_sym (HTML_t *html, const char *sym);
extern int HTML_cmd (HTML_t *html, va_list list);
extern int HTML_env (HTML_t *html, int flag, va_list list);

extern io_t *html_open (const char *dir, const char *path);

#endif	/* HTML.h */
