/*	Arbeiten mit Pixeldateien
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#ifndef _EFEU_Pixmap_h
#define _EFEU_Pixmap_h	1

#include <EFEU/refdata.h>
#include <EFEU/io.h>

typedef struct {
	REFVAR;		/* Referenzvariablen */
	size_t rows;	/* Zahl der Zeilen */
	size_t cols;	/* Zahl der Spalten */
	uchar_t *data;	/* Datenvektor */
} Pixmap_t;

extern reftype_t Pixmap_reftype;

Pixmap_t *NewPixmap (size_t rows, size_t cols, int color);

Pixmap_t *LoadPixmap (io_t *io);
void SavePixmap (Pixmap_t *pix, io_t *io);

void Pixmap_vadjust (Pixmap_t *pix, int idx, int offset);
void SetupPixmap (void);

#endif	/* EFEU/Pixmap.h */
