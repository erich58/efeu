/*	Bilderverwaltung
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_H_PIXMAP
#define	_H_PIXMAP	1

#include <EFEU/efmain.h>
#include <EFEU/object.h>
#include <EFEU/refdata.h>

#define	MSG_PIXMAP	"pixmap"
#define	PIX_CDIM	0xFF	/* Zahl der Farben */

typedef struct {
#if	REVBYTEORDER
	uchar_t blue;
	uchar_t green;
	uchar_t red;
	uchar_t idx;
#else
	uchar_t idx;
	uchar_t red;
	uchar_t green;
	uchar_t blue;
#endif
} Color_t;

int ColorValue(double x);
Color_t SetColor(double x, double y, double z);
int GetColor(Color_t color, Color_t *tab, size_t dim);

typedef struct {
	REFVAR;		/* Referenzzähler */
	int colors;	/* Farbenzahl */
	int rows;	/* Zeilenzahl */
	int cols;	/* Spaltenzahl */
	Color_t color[PIX_CDIM];	/* Farbvektor */
	uchar_t *pixel;	/* Punktefeld */
} OldPixMap_t;

extern reftype_t OldPixMap_reftype;

OldPixMap_t *new_OldPixMap (int rows, int cols);

#define	refer_OldPixMap(pix)	rd_refer(pix)
#define	deref_OldPixMap(pix)	rd_deref(pix)

OldPixMap_t *load_OldPixMap (const char *name);
void save_OldPixMap (OldPixMap_t *pix, const char *name);
void print_OldPixMap (OldPixMap_t *pix, FILE *file);

OldPixMap_t *read_PPMFile (FILE *file);
void OldPixMapToPPM (OldPixMap_t *pix, const char *name);
void OldPixMapToXPM2 (OldPixMap_t *pix, const char *name);


/*	Schnittstelle zu Befehlsinterpreter
*/

extern Type_t Type_OldPixMap;
extern Type_t Type_Color;

extern Color_t Buf_Color;

#define	Obj_OldPixMap(x)	NewPtrObj(&Type_OldPixMap, (x))
#define	Obj_Color(x)	NewObj(&Type_Color, (Buf_Color = (x), &Buf_Color))

#define	Val_OldPixMap(x)	((OldPixMap_t **) x)[0]
#define	Val_Color(x)	((Color_t *) x)[0]


void OldPixMap_Histogramm (Func_t *func, void *rval, void **arg);
void OldPixMap_Slice (Func_t *func, void *rval, void **arg);
void OldPixMap_Dither (Func_t *func, void *rval, void **arg);
void OldPixMap_TeX (Func_t *func, void *rval, void **arg);


/*	Initialisierung
*/

void SetupOldPixMap (void);

#endif /* _H_PIXMAP */

