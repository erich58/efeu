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

#ifndef	_src2doc_h
#define	_src2doc_h	1

#include <EFEU/io.h>
#include <ctype.h>

typedef void (*S2DEval_t) (const char *name, io_t *ein, io_t *aus);

extern S2DEval_t S2DName_get (const char *name);
extern S2DEval_t S2DMode_get (const char *name);
extern void S2DMode_list (io_t *io);

extern void s2d_hdr (const char *name, io_t *ein, io_t *aus);
extern void s2d_src (const char *name, io_t *ein, io_t *aus);
extern void s2d_std (const char *name, io_t *ein, io_t *aus);
extern void s2d_com (const char *name, io_t *ein, io_t *aus);
extern void s2d_doc (const char *name, io_t *ein, io_t *aus);
extern void s2d_man (const char *name, io_t *ein, io_t *aus);

extern int skip_blank (io_t *ein);
extern int skip_space (io_t *ein, strbuf_t *buf);
extern int test_key (io_t *ein, const char *key);

extern strbuf_t *parse_open (void);
extern char *parse_close (strbuf_t *buf);

extern char *parse_name (io_t *io, int c);
extern char *parse_block (io_t *io, int beg, int end);

extern void copy_block (io_t *ein, io_t *aus, int end);
extern void ppcopy (io_t *ein, strbuf_t *val, strbuf_t *com);
extern void copy_protect (const char *str, io_t *io);

/*	Sourcefileanalyse
*/

typedef struct SrcData_s SrcData_t;

typedef struct {
	char *name;
	void (*eval) (SrcData_t *data, const char *name);
} SrcCmd_t;

#define	BUF_DESC	0
#define	BUF_EX		1
#define	BUF_SEE		2
#define	BUF_DIAG	3
#define	BUF_NOTE	4
#define	BUF_WARN	5
#define	BUF_ERR		6
#define	BUF_DIM		7

#define	VAR_NAME	0
#define	VAR_SEC		1
#define	VAR_TITLE	2
#define	VAR_HEAD	3
#define	VAR_COPYRIGHT	4
#define	VAR_DIM		5

struct SrcData_s {
	int hdr;	/* Flag für Header */
	io_t *ein;	/* Eingabestruktur */
	io_t *aus;	/* Ausgabestruktur */
	strbuf_t *buf;	/* Stringbuffer */
	strbuf_t *synopsis;	/* Übersicht */
	strbuf_t *tab[BUF_DIM];	/* Stringbuffertabelle */
	char *var[VAR_DIM];	 /* Variablentabelle */
	SrcCmd_t *ppdef;	/* Preprozessordefinitionen */
	size_t ppdim;	/* Zahl der Preprozessdefinitionen */
	unsigned mask;	/* Maske für Deklarationen */
};

extern void SrcData_init (SrcData_t *data, io_t *ein, io_t *aus);
extern void SrcData_head (SrcData_t *data, const char *name, int sec);
extern void SrcData_eval (SrcData_t *data);
extern void SrcData_clean (SrcData_t *data);
extern void SrcData_copy (SrcData_t *data, strbuf_t *buf);

#define	DECL_VAR	01	/* Variable */
#define	DECL_FUNC	02	/* Funktion */
#define	DECL_LABEL	04	/* Label */
#define	DECL_CALL	010	/* Makroaufruf */
#define	DECL_TYPE	020	/* Typedefinition */
#define	DECL_STRUCT	040	/* Strukturdefinition */

typedef struct {
	int type;	/* Deklarationstype */
	char *def;	/* Deklarationsname */
	char *arg;	/* Vektordimenson/Argumentliste */
	int start;	/* Beginn des letzten Namens */
	int end;	/* Ende des letzten Namens */
} Decl_t;

extern int Decl_test (Decl_t *decl, const char *name);
extern void Decl_print (Decl_t *decl, io_t *io);
extern Decl_t *parse_decl (io_t *io, int c);

extern char *Secnum;
extern char *IncFmt;

#endif	/* src2doc.h */
