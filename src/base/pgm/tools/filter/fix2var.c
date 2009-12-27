#include <stdio.h>

#define	ERR_1	"Unerwartetes Fileende in  Blockgröße.\n"
#define	ERR_2	"Ungültige Blockgröße %d.\n"
#define	ERR_3	"Fehler im Blocklängenfeld %d != 0.\n"

static unsigned block_size(FILE *ein)
{
	unsigned size;
	int c;

	if	((c = getc(ein)) == EOF)	return 0;

	size = (c << 8);

	if	((c = getc(ein)) == EOF)
		return fprintf(stderr, ERR_1), 0;

	size += c;

	if	(size <= 4)
		return fprintf(stderr, ERR_2, (int) size), 0;

	if	((c = getc(ein)) != 0)
		return fprintf(stderr, ERR_3, c), 0;

	if	((c = getc(ein)) != 0)
		return fprintf(stderr, ERR_3, c), 0;

	return size - 4;
}

int main(int narg, char **arg)
{
	int c;
	unsigned n, k, s;

	for (n = k = 0; (s = block_size(stdin)) > 0; n++)
	{
		n++;
		k += s;

		for (; s > 0; s--)
		{
			c = getc(stdin);
			putc(c, stdout);
		}
	}

	fprintf(stderr, "%d Blöcke gelesen, %d Byte ausgegeben.\n", n, k);
	return 0;
}
