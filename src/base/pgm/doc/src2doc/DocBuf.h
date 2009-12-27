/*
Dokumentbuffer

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

#ifndef	_DocBuf_h
#define	_DocBuf_h	1

#include <EFEU/io.h>
#include <EFEU/strbuf.h>

#define	BUF_SYN		0
#define	BUF_DESC	1
#define	BUF_EX		2
#define	BUF_SEE		3
#define	BUF_DIAG	4
#define	BUF_NOTE	5
#define	BUF_WARN	6
#define	BUF_ERR		7
#define	BUF_DIM		8

#define	VAR_NAME	0
#define	VAR_SEC		1
#define	VAR_TITLE	2
#define	VAR_HEAD	3
#define	VAR_COPYRIGHT	4
#define	VAR_DIM		5

typedef struct {
	strbuf_t *synopsis;	/* Übersicht */
	strbuf_t *source;	/* Sourcezeilen */
	strbuf_t *tab[BUF_DIM];	/* Stringbuffertabelle */
	char *var[VAR_DIM];	/* Variablentabelle */
} DocBuf_t;

extern void DocBuf_init (DocBuf_t *doc);
extern void DocBuf_write (DocBuf_t *doc, io_t *io);
extern void DocBuf_copy (DocBuf_t *doc, strbuf_t *buf, strbuf_t *def);

extern void copy_protect (const char *str, io_t *io);

#endif	/* DocBuf.h */
