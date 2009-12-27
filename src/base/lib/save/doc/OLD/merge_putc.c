/*	Zeichen ausgeben
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Document.h>
#include <ctype.h>

#define	LABELNAME	"label"
#define	CMD_NEWPAR	"_newpar"
#define	CMD_ENDPAR	"_endpar"

static void new_par (Merge_t *merge)
{
	Merge_plaincmd(merge, CMD_NEWPAR);
	merge->hmode = HMODE_LINE;
}

static void end_par (Merge_t *merge)
{
	Merge_plaincmd(merge, CMD_ENDPAR);
	merge->hmode = 0;
}

/*	Zeichen ausgeben
*/

int Merge_putc (int c, Merge_t *merge)
{
	return io_putc(c, merge->output);
}

int Merge_xputc (int c, Merge_t *merge)
{
	if	(c == '\n')
	{
		if	(merge->hmode == 0)
		{
			if	(!merge->obbeyline)
				return c;

			new_par(merge);
		}

		if	(merge->hmode > HMODE_LINE)
			io_putc(c, merge->output);

		if	(merge->obbeyline || merge->hmode == HMODE_LINE)
			end_par(merge);
		else
			merge->hmode = HMODE_LINE;

		return c;
	}

/*	Tabulatoren und Leerzeichen auf Ausgabe prüfen
*/
	if	(c == ' ' || c == '\t')
	{
		if	(merge->obbeyspace)		;
		else if	(merge->hmode <= HMODE_WORD)	return c;
		else					c = ' ';
	}

	if	(merge->hmode == 0)
		new_par(merge);

	merge->hmode = (c == ' ' || c == '\t') ? HMODE_WORD : HMODE_TEXT;
	io_putc(c, merge->output);
	return c;
}
