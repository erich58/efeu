/*	Standardz�hler
	(c) 1994 Erich Fr�hst�ck
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
	"Standardz�hler",
	NULL,
	NULL,
	add,
};

MdCount_t *stdcount = &counter;
