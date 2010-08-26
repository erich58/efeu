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

#ifndef	DocOut_h
#define	DocOut_h	1

#include <EFEU/io.h>

typedef struct {
	char *name;
	char *desc;
	IO *(*filter) (IO *io);
	IO *(*docopen) (const char *name, const char *par);
	char *par;
} DocType;

extern IO *DocOut_html (IO *io);
extern IO *DocOut_test (IO *io);
extern IO *DocOut_latex (IO *io);
extern IO *DocOut_mroff (IO *io);
extern IO *DocOut_term (IO *io);

extern char *DocCmdName (int key);
extern char *DocEnvName (int key);

void AddDocType (DocType *type);
DocType *GetDocType (const char *name);
extern IO *DocOut (DocType *type, const char *name);
extern IO *DocFilter (DocType *type, IO *io);
void DocOutInfo (const char *name, const char *desc);

#endif	/* DocOut.h */
