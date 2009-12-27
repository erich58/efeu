/*	Pixelfile in PPM-File konvertieren
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/oldpixmap.h>

void OldPixMapToPPM(OldPixMap_t *pm, const char *name)
{
	int i, j;
	uchar_t *p;
	FILE *file;

	if	(pm == NULL)	return;

	file = fileopen(name, "w");
	fprintf(file, "P3\n%d %d\n255\n", pm->cols, pm->rows);
	p = pm->pixel;

	for (i = 0; i < pm->rows; i++)
	{
		for (j = 0; j < pm->cols; j++)
		{
			if	(j == 0)	;
			else if	(j % 6 == 0)	putc('\n', file);
			else			putc(' ', file), putc(' ', file);

			fprintf(file, "%d %d %d", pm->color[*p].red,
				pm->color[*p].green, pm->color[*p].blue);
			p++;
		}

		putc('\n', file);
	}

	fileclose(file);
}
