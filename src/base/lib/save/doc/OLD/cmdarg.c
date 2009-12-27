/*	Befehlsargumente
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/EisDoc.h>

strbuf_t *EisDocArg (EisDocMerge_t *merge, int endkey)
{
	strbuf_t *buf = new_strbuf(0);
	EisDocOutput_push(&merge->output, io_strbuf(buf));
	EisDocInput_push(&merge->input, NULL);
	merge->input->enc = NULL;
	EisDocMerge(merge, endkey);
	EisDocOutput_pop(&merge->output);
	sb_putc(0, buf);
	return buf;
}

strbuf_t *EisDocStdArg (EisDocMerge_t *merge, int flag)
{
	int c = io_getc(merge->input->io);

	if	(c == '{')
	{
		return EisDocArg(merge, '}');
	}
	else if	(flag)
	{
		while (c == ' ' || c == '\t')
			c = io_getc(merge->input->io);

		return EisDocArg(merge, '\n');
	}

	io_ungetc(c, merge->input->io);
	io_error(merge->input->io, MSG_EISDOC, 2, 0);
	return new_strbuf(0);
}

strbuf_t *EisDocOptArg (EisDocMerge_t *merge)
{
	int c = io_getc(merge->input->io);

	if	(c == '[')
		return EisDocArg(merge, ']');

	io_ungetc(c, merge->input->io);
	return new_strbuf(0);
}
