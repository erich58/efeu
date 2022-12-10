/*
Pixelfile in Graustufen umwandeln

$Copyright (C) 2022 Erich Frühstück
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
#include <EFEU/object.h>
#include <EFEU/stdtype.h>

void Pix_gray(EPixmap *pix)
{
	if	(!pix)
		return;

	size_t size = pix->height * pix->width;
	size_t n;
	unsigned char *p = pix->data;

	for (n = 0; n < size; ++n)
	{
		int g = (p[0] + p[1] + p[2] + 1) / 3;
		p[0] = g;
		p[1] = g;
		p[2] = g;
		p += 3;
	}
}

#define	NGRAY	256
#define	SPACE	3
#define	HSIZE	(SPACE + NGRAY + SPACE)

typedef struct {
	int left;
	int right;
} Maximum;

static int left_margin(int *hist, int m1)
{
	int w1 = 0;
	int w2 = 0;

	for (int i = 1; i <= SPACE; ++i)
	{
		w1 += hist[m1 - i];
		w2 += hist[m1 + i];
	}

	while (w1 && w1 < w2)
	{
		w2 -= hist[m1 + SPACE];
		w2 += hist[m1];
		m1--;
		w1 -= hist[m1];
		w1 += hist[m1 - SPACE];
	}

	return m1;
}

static int right_margin(int *hist, int m2)
{
	int w1 = 0;
	int w2 = 0;

	for (int i = 1; i <= SPACE; ++i)
	{
		w1 += hist[m2 - i];
		w2 += hist[m2 + i];
	}

	while (w2 && w1 > w2)
	{
		w1 -= hist[m2 - SPACE];
		w1 += hist[m2];
		m2++;
		w2 -= hist[m2];
		w2 += hist[m2 + SPACE];
	}

	return m2;
}

static Maximum getmax(int *hist, int start, int end)
{
	int value = 0;
	int m = 0;
	int mw = 0;
	int count = 0;
	Maximum max;

	for (int i = start; i < end; ++i)
	{
		if	(hist[i] == 0)
			continue;

		if	(hist[i] > value)
		{
			m = i;
			value = hist[i];
		}

		mw += hist[i];
		count++;
	}

	if	(!count)
	{
		max.left = start;
		max.right = end - 1;
		return max;
	}

//	fprintf(stderr, "%3d: %5d %5d\n", m - SPACE, value, mw / count);
	max.left = m;
	max.right = m;

	for (int i = 0; i < SPACE; ++i)
	{
		if	(hist[max.left - 1] > hist[max.right + 1])
			max.left--;
		else
			max.right++;
	}

	max.left = left_margin(hist, max.left);
	max.right = right_margin(hist, max.right);
	return max;
}

void Pix_grayhist(EPixmap *pix, IO *out)
{
	if	(!pix || !out)
		return;

	int hist[HSIZE];
	memset(hist, 0, sizeof hist);

	size_t size = pix->height * pix->width;
	unsigned char *p = pix->data;

	for (size_t n = 0; n < size; ++n)
	{
		int g = (p[0] + p[1] + p[2] + 1) / 3;
		hist[SPACE + g]++;
		p += 3;
	}

	for (int i = 0; i < HSIZE; ++i)
	{
		io_printf(out, "%3d %5d\n", i - SPACE, hist[i]);
	}
}

void Pix_bw(EPixmap *pix)
{
	if	(!pix)
		return;

	size_t size = pix->height * pix->width;
	unsigned char *p = pix->data;
	int hist[HSIZE];
	memset(hist, 0, sizeof(hist));

	for (size_t n = 0; n < size; ++n)
	{
		int g = (p[0] + p[1] + p[2] + 1) / 3;
		hist[g]++;
		p += 3;
	}

	Maximum white = getmax(hist, SPACE, SPACE + NGRAY);
	Maximum black = getmax(hist, SPACE, white.left);
	int th = (black.right + white.left) / 2;
	p = pix->data;

	for (size_t n = 0; n < size; ++n)
	{
		int g = (p[0] + p[1] + p[2] + 1) / 3;
		p[0] = p[1] = p[2] = (g <= th) ? 0 : 255;
		p += 3;
	}
}

