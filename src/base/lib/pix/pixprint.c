/*	Pixelfile ausgeben
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/pconfig.h>
#include <ctype.h>
#include <EFEU/oldpixmap.h>

void print_OldPixMap(OldPixMap_t *pix, FILE *file)
{
	int i, j;

	fprintf(file, "PIX %d\n%d %d", pix->colors, pix->rows, pix->cols);

	for (i = 0; i < pix->colors; i++)
	{
		fprintf(file, "\n%d %d %d %d", pix->color[i].idx, pix->color[i].red,
			pix->color[i].green, pix->color[i].blue);
	}

	for (i = 0; i < pix->rows; i++)
	{
		for (j = 0; j < pix->cols; j++)
		{
			putc(j % 32 == 0 ? '\n' : ' ', file);
			fprintf(file, "%d", pix->pixel[j + i * pix->cols]);
		}
	}

	putchar('\n');
}
