/*	Scanner-Ausgabe korrigieren
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#ifndef _korrscan_h
#define _korrscan_h	1

#include <EFEU/memalloc.h>

typedef struct {
	unsigned red : 8;
	unsigned green : 8;
	unsigned blue : 8;
	unsigned unused : 8;
} Point_t;

typedef struct {
	char *label;
	size_t cols;
	size_t rows;
	size_t maxval;
	Point_t *point;
} Picture_t;

Picture_t *AllocPicture (size_t rows, size_t cols);
void FreePicture (Picture_t *pic);

Picture_t *LoadPicture (FILE *file);
Picture_t *CopyPicture (Picture_t *pic, void (*func) (Point_t *p));
void SavePicture (Picture_t *pic, FILE *file);
void ShowPicture (Picture_t *pic, FILE *file);

typedef void (*Adjust_t) (Point_t *point, Point_t *base);
void AdjustPicture (Picture_t *pic, Adjust_t adjust, int n);

void Adjust_red (Point_t *y, Point_t *x);
void Adjust_green (Point_t *y, Point_t *x);
void Adjust_blue (Point_t *y, Point_t *x);

#endif	/* korrscan.h */
