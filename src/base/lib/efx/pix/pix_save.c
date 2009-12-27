/*	Piuxeldatei ausgeben
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Pixmap.h>


void SavePixmap (Pixmap_t *pix, io_t *io)
{
	if	(!pix || !io)	return;

	io_printf(io, "P6\n%d %d\n255\n", pix->cols, pix->rows);
	io_dbwrite(io, pix->data, 1, 1, pix->rows * pix->cols * 3);
}
