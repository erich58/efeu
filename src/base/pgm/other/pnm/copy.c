/*	Bild mit Falrbkorrektur kopieren
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/ftools.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/strbuf.h>
#include <ctype.h>
#include "korrscan.h"

Picture_t *CopyPicture (Picture_t *pic, void (*func) (Point_t *p))
{
	if	(pic)
	{
		Picture_t *tg = AllocPicture(pic->rows, pic->cols);
		tg->label = mstrcpy(pic->label);

		if	(func)
		{
			Point_t *p = pic->point;
			size_t n = pic->rows * pic->cols;

			while (n-- > 0)
				func(p++);
		}

		return tg;
	}
	else	return NULL;
}
