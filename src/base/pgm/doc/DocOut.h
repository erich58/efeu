/*
Dokumentausgabefilter

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

#ifndef	_EFEU_DocOut_h
#define	_EFEU_DocOut_h	1

#include <EFEU/io.h>

typedef struct {
	char *name;
	char *desc;
	io_t *(*filter) (io_t *io);
	io_t *(*docopen) (const char *name, const char *par);
	char *par;
} DocType_t;

extern io_t *DocOut_html (io_t *io);
extern io_t *DocOut_test (io_t *io);
extern io_t *DocOut_latex (io_t *io);
extern io_t *DocOut_mroff (io_t *io);
extern io_t *DocOut_term (io_t *io);

extern char *DocCmdName (int key);
extern char *DocEnvName (int key);

DocType_t *GetDocType (const char *name);
extern io_t *DocOut (DocType_t *type, const char *name);
extern io_t *DocFilter (DocType_t *type, io_t *io);
void DocOutInfo (const char *name, const char *desc);

#endif	/* EFEU/DocOut.h */
