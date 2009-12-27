/*	Hilfsroutinen fuer Aggregatfile-Datenformat
	(c) 1993 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6

	Version 2.0
*/

#include "oestz.h"

static char input_buf[OESTZ_RECLEN + 1];
static char input_flag = 0;
static io_t *input = NULL;


/*	OESTZ - Datei oeffnen
*/

void oestz_open(const char *name)
{
	input = io_fileopen(name, "rz");
}

void oestz_close(void)
{
	io_close(input);
	input = NULL;
}

/*	Teilstring parsen
*/

char *oestz_parse(const char *p, int first, int last)
{
	static char buf[OESTZ_RECLEN + 1];
	int i;

	first--;
	last -= first;

	for (i = 0; i < last; i++)
		buf[i] = p[first + i];

	do	buf[i--] = 0;
	while	(buf[i] == ' ' && i >= 0);

	return buf;
}


/*	OESTZ - Datensatz lesen
*/

char *oestz_get(int type, int flag)
{
	if	(input_flag == 0)
	{
		switch (io_read(input, input_buf, OESTZ_RECLEN))
		{
		case 0:

			input_flag = EOF;
			break;

		case OESTZ_RECLEN:

			input_buf[OESTZ_RECLEN] = 0;
			break;

		default:

			reg_set(1, msprintf("%8lu", OESTZ_RECLEN));
			liberror(OESTZ, 1);
			break;
		}
	}

/*	Test, ob Satz die gewünschten Anforderungen erfüllt
*/
	if	(input_flag == EOF)
	{
		return NULL;
	}
	else if	(type == 0 || type == input_buf[0])
	{
		input_flag = 0;
		return input_buf;
	}
	else if	(flag)
	{
		reg_set(1, msprintf("%8lu", OESTZ_RECLEN));
		reg_set(2, msprintf("%#c", type));
		liberror(OESTZ, 2);
	}
	else	input_flag = 1;

	return NULL;
}
