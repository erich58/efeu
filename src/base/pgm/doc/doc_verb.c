/*	Wörtwörtliche Ausgabe
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/mstring.h>
#include <efeudoc.h>


void Doc_verb (Doc_t *doc, io_t *in, int base, int alt)
{
	int c;
	int pos;

	Doc_start(doc);

	if	(base)
		io_ctrl(doc->out, DOC_BEG, base, alt);

	pos = 0;

	while ((c = io_getc(in)) != EOF)
	{
		switch (c)
		{
		case 0:
		case 127:
		case '\b':
		case '\r':
		case '\f':
		case '\v':
			continue;
		case '\n':
			io_putc(c, doc->out);
			pos = 0;
			break;
		case '\t':
			pos += io_nputc(' ', doc->out, 8 - pos % 8);
			break;
		default:
			io_putc(c, doc->out);
			pos++;
			break;
		}
	}

	if	(base)
		io_ctrl(doc->out, DOC_END, base, alt);
}
