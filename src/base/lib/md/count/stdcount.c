/*	Standardzähler
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2
*/

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>

static void add(void *data)
{
	Val_int(data)++;
}

static MdCount_t counter = {
	"std",
	"int",
	"Standardzähler",
	NULL,
	NULL,
	add,
};

MdCount_t *stdcount = &counter;
