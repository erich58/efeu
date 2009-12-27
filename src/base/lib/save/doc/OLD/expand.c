/*	Mischbefehle expandieren
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Document.h>
#include <ctype.h>

#define	TODO	0

int Merge_expand (Merge_t *merge, int c)
{
	if	(c != '\\')	return 0;

	c = io_getc(merge->input);

	switch (c)
	{
	case EOF:
	case '\\':
		return 0;
	case ';':
	case '\n':
		return 1;
	default:
		break;
	}

	if	(isalpha(c) || c == '_')
	{
		char *p = DocParse_name(merge->input, c);

#if	TODO
		if	(!Merge_stdcmd(merge, p))
			Merge_printf(merge, "\\\\%s ", p);
#endif

		memfree(p);
		return 1;
	}

	io_ungetc(c, merge->input);
	return 0;
}
