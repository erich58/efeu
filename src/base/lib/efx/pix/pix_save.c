/*
Pixeldatei ausgeben

$Copyright (C) 2000 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/Pixmap.h>


void SavePixmap (EPixmap *pix, IO *io)
{
	if	(!pix || !io)	return;

	io_xprintf(io, "P6\n%d %d\n255\n", pix->cols, pix->rows);
	io_dbwrite(io, pix->data, 1, 1, pix->rows * pix->cols * 3);
}
