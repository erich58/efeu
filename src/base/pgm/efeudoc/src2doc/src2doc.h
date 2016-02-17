/*
Dokumentation aus Sourcefile generieren

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

#ifndef	src2doc_h
#define	src2doc_h	1

#include <EFEU/io.h>
#include <ctype.h>
#include "DocBuf.h"

typedef void (*S2DEval) (const char *name, IO *ein, IO *aus);

S2DEval S2DName_get (const char *name);
S2DEval S2DMode_get (const char *name);
void S2DMode_list (IO *io);

void s2d_hdr (const char *name, IO *ein, IO *aus);
void s2d_xhdr (const char *name, IO *ein, IO *aus);
void s2d_src (const char *name, IO *ein, IO *aus);
void s2d_cmd (const char *name, IO *ein, IO *aus);
void s2d_std (const char *name, IO *ein, IO *aus);
void s2d_xstd (const char *name, IO *ein, IO *aus);
void s2d_script (const char *name, IO *ein, IO *aus);
void s2d_xscript (const char *name, IO *ein, IO *aus);
void s2d_com (const char *name, IO *ein, IO *aus);
void s2d_doc (const char *name, IO *ein, IO *aus);
void s2d_man (const char *name, IO *ein, IO *aus);
void s2d_sh (const char *name, IO *ein, IO *aus);

int skip_blank (IO *ein);
int skip_space (IO *ein, StrBuf *buf);
int test_key (IO *ein, const char *key);

StrBuf *parse_open (void);
char *parse_close (StrBuf *buf);

char *parse_name (IO *io, int c);
char *parse_block (IO *io, int beg, int end, int flag);

void copy_block (IO *ein, IO *aus, int end, int flag);
void copy_token (IO *ein, IO *aus, int c, int flag);
void copy_str (IO *ein, IO *aus, int delim);
void copy_ (IO *ein, IO *aus, int end, int flag);
void ppcopy (IO *ein, StrBuf *val, StrBuf *com);

/*	Sourcefileanalyse
*/

typedef struct SrcDataStruct SrcData;

typedef struct {
	char *name;
	void (*eval) (SrcData *data, const char *name);
} SrcCmd;


struct SrcDataStruct {
	int hdr;	/* Flag für Header */
	IO *ein;	/* Eingabestruktur */
	StrBuf buf;	/* Stringbuffer */
	DocBuf doc;	/* Dokumentbuffer */
	SrcCmd *ppdef;	/* Preprozessordefinitionen */
	size_t ppdim;	/* Zahl der Preprozessdefinitionen */
	unsigned mask;	/* Maske für SYNOPSIS-Deklarationen */
	unsigned xmask;	/* Maske für beschriebene Deklarationen */
};

void SrcData_init (SrcData *data, IO *ein);
void SrcData_head (SrcData *data, const char *name, int sec);
void SrcData_title (SrcData *data, const char *name);
void SrcData_eval (SrcData *data, const char *name);
StrBuf *SrcData_copy (SrcData *data, StrBuf *buf, const char *name);
void SrcData_write (SrcData *data, IO *io);

#define	DECL_VAR	01	/* Variable */
#define	DECL_FUNC	02	/* Funktion */
#define	DECL_LABEL	04	/* Label */
#define	DECL_SVAR	010	/* statische Variable */
#define	DECL_SFUNC	020	/* statische Funktion */
#define	DECL_CALL	040	/* Makroaufruf */
#define	DECL_TYPE	0100	/* Typedefinition */
#define	DECL_STRUCT	0200	/* Strukturdefinition */

typedef struct {
	int type;	/* Deklarationstype */
	char *def;	/* Deklarationsname */
	char *arg;	/* Vektordimenson/Argumentliste */
	int start;	/* Beginn des letzten Namens */
	int end;	/* Ende des letzten Namens */
} Decl;

int Decl_test (Decl *decl, const char *name);
void Decl_print (Decl *decl, IO *io, const char *name);
Decl *parse_decl (IO *io, int c);

void MacDef_clean (void);
void MacDef_add (IO *ein);
char *MacDef_name (void);
void MacDef_source (StrBuf *sb);
void MacDef_synopsis (StrBuf *sb);
void MacDef_list (StrBuf *sb);

extern char *Secnum;
extern char *IncFmt;
extern int InsertCode;

#endif	/* src2doc.h */
