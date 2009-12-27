/*	Standardausgabefilter für Dokument
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/DocOut.h>
#include <ctype.h>

#define	STAT_NL	1	/* Zeilenvorschub gelesen */
#define	STAT_SP	2	/* Leerzeichen gelesen */
#define	STAT_M1	3	/* Ein Minus gelesen */
#define	STAT_P1	4	/* Ein Plus gelesen */
#define	STAT_M2	5	/* Zwei Minus gelesen */

/*	ACHTUNG: Da die Ausgabefunktionen den Filter zum Teil wieder
	aufrufen, muß zur Vermeidung einer unendlicher Rekursion die
	Variable out->fstat vorher auf 0 gesetzt werden.
*/

static void end_stat (DocOut_t *out, int stat)
{
	out->fstat = 0;

	switch (stat)
	{
	case STAT_M1:	
		DocOut_beg(out, DOCPOS_WORD);
		DocOut_char('-', out, 1);
		break;
	case STAT_P1:	
		DocOut_beg(out, DOCPOS_WORD);
		DocOut_char('+', out, 1);
		break;
	case STAT_M2:	
		DocOut_symbol(out, "endash");
		break;
	case STAT_NL:	
	case STAT_SP:	
		DocOut_sync(out, DOCPOS_LINE);
		DocOut_char((out->wrap && out->col >= out->wrap) ?
			'\n' : ' ', out, 1);
		break;
	default:
		break;
	}
}


int DocFilter_std (int c, DocOut_t *out)
{
/*	Sonderzeichen
*/
	switch (c)
	{
	case 0:

		end_stat(out, out->fstat);
		return 0;

	case '-':

		switch (out->fstat)
		{
		case STAT_M1:
			out->fstat = STAT_M2;
			break;
		case STAT_M2:
			out->fstat = 0;
			DocOut_symbol(out, "emdash");
			break;
		default:
			end_stat(out, out->fstat);
			out->fstat = STAT_M1;
			break;
		}

		return c;

	case '+':

		end_stat(out, out->fstat);
		out->fstat = STAT_P1;
		return c;

	case '\n':

		switch (out->fstat)
		{
		case STAT_NL:
			out->fstat = 0;	/* Notwendig für DocOut_end()! */
			DocOut_end(out, DOCPOS_BODY);
			return 0;
		case STAT_SP:
			out->fstat = STAT_NL;
			return 0;
		default:
			end_stat(out, out->fstat);
			break;
		}

		if	(out->pos > DOCPOS_PAR)
			out->fstat = STAT_NL;

		return 0;

	default:

		break;
	}

/*	Weiße Zeichen
*/
	if	(isspace(c))
	{
		switch (out->fstat)
		{
		case STAT_SP:
		case STAT_NL:
			out->fstat = STAT_SP;
			return 0;
		default:
			end_stat(out, out->fstat);
			break;
		}

		if	(out->pos > DOCPOS_PAR)
			out->fstat = STAT_SP;

		return 0;
	}

/*	Status zurücksetzen
*/
	if	(isdigit(c) || c == '.' || c == ',')
	{
		switch (out->fstat)
		{
		case STAT_P1:
			out->fstat = 0;
			DocOut_symbol(out, "plus");
			break;
		case STAT_M1:
			out->fstat = 0;
			DocOut_symbol(out, "minus");
			break;
		default:
			break;
		}
	}

	end_stat(out, out->fstat);
	DocOut_beg(out, DOCPOS_WORD);
	DocOut_char(c, out, 1);
	return c;
}

