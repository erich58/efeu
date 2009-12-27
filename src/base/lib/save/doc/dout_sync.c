/*	Ausgabeposition synchronisieren
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/DocOut.h>
#include <ctype.h>

#define	BBEG	"body"
#define	BEND	"endbody"
#define	PBEG	"par"
#define	PEND	"endpar"
#define	LBEG	"lbeg"
#define	LEND	"lend"
#define	WBEG	"wbeg"
#define	WEND	"wend"

void DocOut_beg (DocOut_t *out, int pos)
{
	while (out->pos < pos)
	{
		out->pos++;

		switch (out->pos)
		{
		case DOCPOS_BODY:
			DocOut_ctrl(out, BBEG, NULL);
			break;
		case DOCPOS_PAR:
			DocOut_ctrl(out, PBEG, NULL);
			break;
		case DOCPOS_LINE:
			DocOut_ctrl(out, LBEG, NULL);
			break;
		case DOCPOS_WORD:
			DocOut_ctrl(out, WBEG, NULL);
			break;
		default:	
			break;
		}
	}
}

void DocOut_end (DocOut_t *out, int pos)
{
	if	(out->filter)
		out->filter(0, out);

	while (out->pos > pos)
	{
		switch (out->pos)
		{
		case DOCPOS_WORD:
			DocOut_ctrl(out, WEND, NULL);
			break;
		case DOCPOS_LINE:
			DocOut_ctrl(out, LEND, NULL);

			if	(out->col)
				DocOut_char('\n', out, 1);

			break;
		case DOCPOS_PAR:
			DocOut_ctrl(out, PEND, NULL);
			break;
		case DOCPOS_BODY:
			DocOut_ctrl(out, BEND, NULL);
			break;
		default:
			break;
		}

		out->pos--;
	}
}

void DocOut_sync (DocOut_t *out, int pos)
{
	DocOut_end(out, pos);
	DocOut_beg(out, pos);
}
