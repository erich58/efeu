/*	Hilfsprogramme zur TeX-Aufbereitung
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6
*/

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/mdmat.h>

#ifndef	TEXUTIL_H
#define	TEXUTIL_H	1

typedef struct {
	int size;	/* Basisgr��e */
	char *name;	/* Schriftname */
	double width;	/* Ziffernbreite in mm */
	double height;	/* Zeilenh�he in mm */
} TeXfont_t;


TeXfont_t *TeXfont (int psize);

/*
void TeXfontinit (void);
void TeXfontwalk (int (*visit) (TeXfont_t *font));
*/
void *TeXfontlist (Func_t *func, void *data, void **arg);

typedef struct {
	char *name;	/* Papiername */
	double margin;	/* Linker Rand in mm */
	double topskip;	/* Oberer Rand in mm */
	double width;	/* Papierbreite in mm */
	double height;	/* Papierh�he in mm */
} TeXpgfmt_t;


TeXpgfmt_t *TeXpgfmt (const char *name);
void *TeXpglist (Func_t *func, void *data, void **arg);

void TeXbegin (io_t *io, TeXpgfmt_t *pgfmt);
void TeXend (io_t *io);
extern int TeXcompletion;

#endif	/* TEXUTIL_H */
