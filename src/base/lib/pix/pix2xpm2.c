/*	Pixelfile in XPM2-File konvertieren
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/oldpixmap.h>

void OldPixMapToXPM2(OldPixMap_t *pix, const char *name)
{
	int i, j;
	char *fmt;
	FILE *file;
	int n;

	file = fileopen(name, "w");

	if	(pix->colors <= 16)
	{
		fmt = "%X";
		n = 1;
	}
	else
	{
		fmt = "%02X";
		n = 2;
	}

	fprintf(file, "/* XPM2 C */\n static char ** bixmap = {\n\"");
	fprintf(file, "%d %d %d %d", pix->cols, pix->rows, pix->colors, n);

	for (i = 0; i < pix->colors; i++)
	{
		fputs("\",\n\"", file);
		fprintf(file, fmt, i);
		fprintf(file, " m C%d c #%02X%02X%02X", i, pix->color[i].red,
			pix->color[i].green, pix->color[i].blue);
	}

	for (i = 0; i < pix->rows; i++)
	{
		fputs("\",\n\"", file);

		for (j = 0; j < pix->cols; j++)
			fprintf(file, fmt, pix->pixel[i*pix->cols + j]);
	}

	fputs("\"}\n", file);
	fileclose(file);
}
