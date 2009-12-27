/*
Datenblock im Stringbuffer verschieben
*/

#include <EFEU/strbuf.h>

int sb_swap (StrBuf *sb, int pos, int n)
{
	if	(sb && pos + sb->nfree + n <= sb->size)
	{
		unsigned char *a = sb->data + pos;
		unsigned char *b = a + n - 1;

		while (a < b)
		{
			unsigned char c = *a;
			*a = *b;
			*b = c;
			a++;
			b--;
		}
		
		return n;
	}

	return 0;
}
