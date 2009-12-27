/*	Mischstruktur
	(c) 1998 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 1.0
*/

#include "eisdoc.h"

/*	Mischzustand
*/

Merge_t MergeStat = MERGE_DATA('@', 0);

void PushMerge (Merge_t *save)
{
	*save = MergeStat;
	rd_refer(MergeStat.in);
	rd_refer(MergeStat.out);
	MergeStat.buf = NULL;
}

void PopMerge (Merge_t *save)
{
	rd_deref(MergeStat.in);
	rd_deref(MergeStat.out);

	if	(MergeStat.buf)
		del_strbuf(MergeStat.buf);

	MergeStat = *save;
}

/*	Hilfsfunktionen
*/

void MergeInput (io_t *in)
{
	rd_deref(MergeStat.in);
	MergeStat.in = in;
	MergeStat.linestart = 1;
}

void MergeOutput (io_t *out)
{
	rd_deref(MergeStat.out);
	MergeStat.out = out;
}

char *MergeComment (void)
{
	if	(MergeStat.buf)
	{
		char *p = sb2str(MergeStat.buf);
		MergeStat.buf = NULL;
		return p;
	}
	
	return NULL;
}

void MergeNewline(void)
{
	if	(MergeStat.outchar)
	{
		io_putc('\n', MergeStat.out);
		MergeStat.pos = 0;
		MergeStat.outchar = 0;
	}
}

void MergeCtrl(int c)
{
	io_putc(c, MergeStat.out);

	if	(c == '\n')
	{
		MergeStat.pos = 0;
		MergeStat.outchar = 0;
	}
	else	MergeStat.outchar = 1;
}

void MergePutc(int c)
{
	if	(c == '\n')
	{
		MergeCtrl(c);
		return;
	}
	else if	(c == '\t')
	{
		do	MergePutc(' ');
		while	(MergeStat.pos % 8 != 0);

		return;
	}
	else if	(MergeStat.filter)
	{
		MergeStat.filter->put(c, MergeStat.out);
	}
	else	io_putc(c, MergeStat.out);

	MergeStat.outchar = 1;
	MergeStat.pos++;
}

void MergePuts(const char *str)
{
	if	(str && MergeStat.out)
	{
		for (; *str != 0; str++)
			MergePutc(*str);
	}
}

void MergePutCmd(const char *name)
{
	if	(name && MergeStat.out && MergeStat.cmdkey)
	{
		MergePutc(MergeStat.cmdkey);
		MergePuts(name);
	}
}

void MergeFilter(const char *name)
{
	MergeStat.filter = name ? GetFilter(name) : NULL;
}
